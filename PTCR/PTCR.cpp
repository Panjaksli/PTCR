#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <imgui.h>
#include <time.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_sdlrenderer.h>
#include "scene.h"
#include "event_handler.h"
#define WIDTH 800
#define HEIGHT 600
#define SCALE 1 //0.6666667f
#define CLAMP ImGuiSliderFlags_AlwaysClamp
#if DEBUG
bool use_normal_maps = 1;
#endif
void scn1(scene& scn) {
	scn.world.clear();
	albedo iron = albedo(texture("iron_block.png"), texture("iron_block_mer.png"), texture("iron_block_normal.png"), 2e2);
	albedo snow = albedo(texture("snow.png"), texture("snow_mer.png"), texture("iron_block_normal.png"), 2e3);
	albedo pbrcol(vec3(0.5, 0.1, 0.1), vec3(), texture("gravel_normal.png"), 10);
	albedo white(vec3(0.8, 0.8, 0.8));
	scn.world.add_mat(iron, mat_ggx);
	scn.world.add_mat(pbrcol, mat_ggx);
	scn.world.add_mat(pbrcol, mat_ggx);
	scn.world.add(quad(vec3(-100, eps, -100), vec3(100, eps, -100), vec3(-100, eps, 100)), 0);
	scn.world.add(sphere(vec3(-1, 0.95f, -3, 1)), 1);
	scn.world.add(sphere(vec3(1, 0.95f, -3, 1)), 2);
	scn.sun_pos.set_A(vec3(1, 0, 0.32));
	scn.cam.setup(matrix(vec3(0, 1, 0), vec3(0, 0, 0)), 70, 1);
}
void scn2(scene& scn) {
	scn.world.clear();
	scn.opt.li_sa = 1;
	albedo iron = albedo(texture("iron_block.png"), texture("iron_block_mer.png"), texture("iron_block_normal.png"), 10);
	albedo white(vec3(0.8, 0.8, 0.8), vec3(0, 5, 0));
	albedo pbrcol(vec3(0.5, 0.1, 0.1), vec3(), texture("iron_block_normal.png"), 10);
	scn.world.add_mat(white, mat_lam);
	scn.world.add_mat(iron, mat_ggx);
	scn.world.add_mat(white, mat_uli);
	vector<quad> room(6);
	room.push_back(quad(vec3(-1, 0, -1), vec3(3, 0, -1), vec3(-1, 0, 1)));
	room.push_back(quad(vec3(-1, 0, -1), vec3(-1, 2, -1), vec3(-1, 0, 1)));
	room.push_back(quad(vec3(3, 0, -1), vec3(3, 1.5, -1), vec3(3, 0, 1)));
	room.push_back(quad(vec3(-1, 2, -1), vec3(3, 2, -1), vec3(-1, 2, 1)));
	room.push_back(quad(vec3(-1, 0, -1), vec3(3, 0, -1), vec3(-1, 2, -1)));
	room.push_back(quad(vec3(-1, 0, 1), vec3(3, 0, 1), vec3(-1, 2, 1)));
	scn.world.add(room, 0);
	scn.world.add(quad(vec3(0, 0, 0), vec3(1, 0, 0), vec3(0, 1, 0.999)), 1); //0.24 0 0.67
	scn.world.add(voxel(vec3(0, 1.9, 0, 0.1)), 2, 1);
	scn.sun_pos.set_A(vec3(-1, 0, 0));
	scn.cam.setup(matrix(vec3(2, 1, 0), vec3(0, hpi, 0)), 90);
}
void scn3(scene& scn) {
	scn.world.clear();
	for (int i = 0; i <= 10; i++) {
		for (int j = 0; j <= 10; j++) {
			albedo pbrcol(vec3(0.9f, 0.6f, 0.), vec3(0.1 * i, 0, 0.1 * j), vec3(0.5, 0.5, 1), 10);
			scn.world.add_mat(pbrcol, mat_vnd);
			scn.world.add(sphere(vec3(6 - j, 6 - i, -3, 0.5)), i * 11 + j);
		}
	}
	scn.sun_pos.set_A(vec3(1, 0, 1));
	scn.cam.setup(matrix(vec3(1, 1, 10), vec3(0, 0, 0)), 47);
}
int main()
{
	uint width = WIDTH, height = HEIGHT;
	SDL_Init(SDL_INIT_EVERYTHING);
#if defined(_WIN32) || defined(WIN32)
	SDL_SetHint(SDL_HINT_RENDER_DRIVER, "direct3d11");
#endif
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");
	SDL_Window* window = SDL_CreateWindow("RTCR", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width * 16 / 12, height, SDL_WINDOW_SHOWN);
	SDL_SetWindowMinimumSize(window, 200, 200);
	SDL_Texture* frame;
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.WantCaptureMouse = 1;
	ImGui::StyleColorsDark();
	ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
	ImGui_ImplSDLRenderer_Init(renderer);
	bool overlay = true;
	SDL_PixelFormatEnum Format;
	Format = SDL_PIXELFORMAT_RGBA32;
	frame = SDL_CreateTexture(renderer, Format, SDL_TEXTUREACCESS_STREAMING, SCALE * width, SCALE * height);
	SDL_Event event;
	event_handler handle;
	bool running = true;
	SDL_Rect out = { 0,0,WIDTH,HEIGHT };
	double frametime = 0;
	double target_fps = 90;
	double mov = 1;
	double mul = 1;
	double target_frametime = 1.f / target_fps;
	double smooth_fps = 0;
	obj_id id;
	scene scn(90, SCALE * width, SCALE * height);
	float& scale = scn.opt.res_scale;
	scale = SCALE;
	bool& moving = scn.cam.moving;
	bool tap_to_focus = 0;
	scn3(scn);
	printf("init done!!!\n");
	matrix T;
	vec3 TP = scn.sun_pos.P();
	vec3 TA = scn.sun_pos.A;
	while (running) {
		frametime = SDL_GetPerformanceCounter();
		SDL_RenderClear(renderer);
		while (SDL_PollEvent(&event))
		{
			ImGui_ImplSDL2_ProcessEvent(&event);
			handle.scan(event);
			if (event.type == SDL_QUIT)
				running = false;
			if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
				running = false;

			if (!io.WantCaptureMouse) {
				if (handle.rmb) {
					int x, y;
					SDL_GetMouseState(&x, &y);
					id = scn.get_id(y, x, T);
					if(tap_to_focus)
					scn.cam_manufocus(y,x);
					TP = T.P();
					TA = T.A;
				}
				if (handle.motion && handle.lmb)
					scn.cam.rotate(0, -torad(0.9 * handle.xvec), -torad(0.9 * handle.yvec));
			}
			if (handle[SDL_SCANCODE_PRINTSCREEN]) {

			}


		}
		if (handle[SDL_SCANCODE_LSHIFT])mul = 2.0;
		else mul = 1.0;
		if (handle[SDL_SCANCODE_W])scn.cam.move_fps(mul * mov, 0, 0);
		if (handle[SDL_SCANCODE_S])scn.cam.move_fps(-mul * mov, 0, 0);
		if (handle[SDL_SCANCODE_A])scn.cam.move_fps(0, 0, -mul * mov);
		if (handle[SDL_SCANCODE_D])scn.cam.move_fps(0, 0, mul * mov);
		if (handle[SDL_SCANCODE_SPACE])scn.cam.move_fps(0, mul * mov, 0);
		if (handle[SDL_SCANCODE_LCTRL])scn.cam.move_fps(0, -mul * mov, 0);

		ImGui_ImplSDLRenderer_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();
		ImGui::SetNextWindowPos(ImVec2(WIDTH, 0));
		ImGui::SetNextWindowSize(ImVec2(WIDTH * 16 / 12 - WIDTH, HEIGHT / 2));
		if (overlay)
		{
			ImGui::Begin("Camera settings");
			if (ImGui::Button("Screenshot")) {
				save_png(scn.image(), scn.cam.w, scn.cam.h);
			}
			static int scene = 3;
			if (ImGui::SliderInt("Scene", &scene, 1, 3)) {
				moving = 1;
				switch (scene) {
				case 1: scn1(scn); break;
				case 2: scn2(scn); break;
				case 3: scn3(scn); break;
				default: scn1(scn); break;
				}
			}
			ImGui::DragFloat("Speed", &scn.cam.speed, 0.001, 0.001, 1e3f, "% .2f", 1.1f);
			if (ImGui::DragFloat("FOV", &scn.cam.fov, 0.1, 0.001, 179, "%.1f", 1.1f))
			{
				moving = 1;
				scn.cam.update();
			}
			if (ImGui::Checkbox("Auto focus", &scn.cam.autofocus))
			{
				if (scn.cam.autofocus)tap_to_focus = 0;
				moving = 1;
			}
			ImGui::SameLine();
			if(ImGui::Checkbox("Tap to focus", &tap_to_focus))
			{
				if (tap_to_focus)scn.cam.autofocus = 0;
			}
			if (ImGui::InputFloat("Foc dist", &scn.cam.foc_t, 0.001, 0.001))
			{
				moving = 1;
				scn.cam.autofocus = 0;
			}
			moving |= ImGui::DragFloat("Exposure", &scn.cam.exposure, 0.1, 0.01, 100.f, "% .2f",1.1f);
			moving |= ImGui::DragFloat("F-stop", &scn.cam.fstop, 0.1, 0.1f, 64.f, "% .2f",1.1f);

			if (ImGui::DragFloat("Res scale", &scale, 0.01, 0.01, 2, " % .2f", CLAMP))
			{
				scn.cam.resize(width, height, scale);
				scn.cam.update();
				SDL_DestroyTexture(frame);
				frame = SDL_CreateTexture(renderer, Format, SDL_TEXTUREACCESS_STREAMING, scn.cam.w, scn.cam.h);
			}
			ImGui::DragFloat("Render Rate", &scn.opt.res_rate, 0.01, 0.01, 1, "%.2f", CLAMP);
			if (ImGui::DragFloat("Ray lifetime", &scn.opt.p_life, 0.01, 0.01, 1.f, "%.2f", CLAMP))
				scn.opt.i_life = 1.f / scn.opt.p_life, moving = 1;
			ImGui::InputInt("Samples", &scn.opt.samples, 1, 1);
			moving |= ImGui::InputInt("Bounces", &scn.opt.bounces, 1, 1);
			moving |= ImGui::Checkbox("Light sampling", &scn.opt.li_sa); ImGui::SameLine();
			moving |= ImGui::Checkbox("Sun sampling", &scn.opt.sun_sa);

#if DEBUG

			moving |= ImGui::Checkbox("Debug Aten", &scn.opt.dbg_at); ImGui::SameLine();
			moving |= ImGui::Checkbox("Debug N", &scn.opt.dbg_n);
			moving |= ImGui::Checkbox("Debug UV", &scn.opt.dbg_uv);	ImGui::SameLine();
			moving |= ImGui::Checkbox("Debug t", &scn.opt.dbg_t);
			moving |= ImGui::Checkbox("Normal maps", &use_normal_maps); ImGui::SameLine();
			moving |= ImGui::Checkbox("Recursion", &scn.opt.recur);
			moving |= ImGui::Checkbox("Perf mode", &scn.opt.p_mode); ImGui::SameLine();
			static bool denoise = 1;
			ImGui::Checkbox("Denoise", &denoise);// ImGui::SameLine();
			moving |= !denoise;
#endif
			ImGui::Text("%.2f ms %.1f FPS,Samples %.1fk", 1000.0f / smooth_fps, smooth_fps, scn.cam.CCD.t * 0.001f);
			ImGui::End();




			ImGui::SetNextWindowPos(ImVec2(WIDTH, HEIGHT / 2));
			ImGui::SetNextWindowSize(ImVec2(WIDTH * 16 / 12 - WIDTH, HEIGHT / 2));


			ImGui::Begin("Object properties");
			ImGui::Text("ID: %d, Type: %d", id.id, id.type);
			if (id.type != o_bla) {
				albedo& alb = scn.world.materials[scn.world.objects[id.id].get_mat()].tex;
				int type = (int)scn.world.materials[scn.world.objects[id.id].get_mat()].type;
				vec3 col = alb.get_rgb(), mer = alb.get_mer();
				float& rep = alb.get_rep();
				moving |= ImGui::SliderInt("Mat", &type, 0, 5);
				moving |= ImGui::InputFloat("Rep", &rep, 1, 10);
				moving |= ImGui::ColorEdit4("Col", col._xyz, ImGuiColorEditFlags_Float);
				moving |= ImGui::ColorEdit3("Mer", mer._xyz, ImGuiColorEditFlags_Float);
				scn.world.materials[scn.world.objects[id.id].get_mat()].type = (mat_enum)type;
				alb.set_rgb(col);
				alb.set_mer(mer);
			}
			if (ImGui::DragFloat3("Pos", TP._xyz, 0.01))
			{
				T.set_P(TP);
				scn.set_trans(id, T);
				moving = 1;
			}
			if (ImGui::DragFloat3("Rot", TA._xyz, 0.01))
			{
				T.set_A(TA);
				scn.set_trans(id, T);
				moving = 1;
			}
			ImGui::End();
		}
		ImGui::Render();

		scn.render();
		SDL_UpdateTexture(frame, NULL, scn.image(), scn.cam.w * 4);

		SDL_RenderCopy(renderer, frame, NULL, &out);
		ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
		SDL_RenderPresent(renderer);
		frametime = (SDL_GetPerformanceCounter() - frametime) / SDL_GetPerformanceFrequency();
		double delay = target_frametime - frametime;
		mov = fmax(frametime, target_frametime) / target_frametime;
		smooth_fps = ImGui::GetIO().Framerate;
		SDL_Delay(fmaxf(0.0, 1000.0 * delay));
		/*scn.opt.res_rate = clamp(smooth_fps / target_fps-0.5, 1e-4, 1);
		scn.opt.spec_rate = clamp(smooth_fps / target_fps-0.5, 1e-4, 1);*/


	}
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}