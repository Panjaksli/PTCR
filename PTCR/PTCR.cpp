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
#if DEBUG
float spec_filter = 10.f;
bool use_normal_maps = 1;
#endif
void scn1(scene& scn) {
	scn.world.clear();
	albedo iron = albedo(texture("iron_block.png"), texture("iron_block_mer.png"), texture("iron_block_normal.png"), 2e3);
	albedo snow = albedo(texture("snow.png"), texture("snow_mer.png"), texture("iron_block_normal.png"), 2e3);
	albedo white(vec3(0.8, 0.8, 0.8));
	albedo pbrcol(vec3(0.5, 0.1, 0.1),vec3(), texture("iron_block_normal.png"),10);
	//auto pbr_iron = pbr(iron);
	//auto pbr_orange = pbr(orange);
	//scn.world.add_mat(iron, mat_ggx);
    scn.world.add_mat(iron, mat_ggx);
	scn.world.add_mat(pbrcol, mat_ggx);
	scn.world.add_mat(pbrcol, mat_vnd);
	//scn.world.add_mat(white, mat_uli);
	//scn.world.add_mat(white,mat_lam);
	/*vector<quad> room;
	room.push_back(quad(vec3(-1, 0, -1), vec3(3, 0, -1), vec3(-1, 0, 1)));
	room.push_back(quad(vec3(-1, 0, -1), vec3(-1, 2, -1), vec3(-1, 0, 1)));
	room.push_back(quad(vec3(3, 0, -1), vec3(3, 1.5, -1), vec3(3, 0, 1)));
	room.push_back(quad(vec3(-1, 2, -1), vec3(3, 2, -1), vec3(-1, 2, 1)));
	room.push_back(quad(vec3(-1, 0, -1), vec3(3, 0, -1), vec3(-1, 2, -1)));
	room.push_back(quad(vec3(-1, 0, 1), vec3(3, 0, 1), vec3(-1, 2, 1)));
	scn.world.add(room, 1);
	scn.world.add(quad(vec3(0,0,0), vec3(1, 0, 0), vec3(0, 1, 0.999)), 0);
	scn.world.add(voxel(vec3(0, 1.9, 0, 0.1)), 3, 1);*/
	scn.world.add(quad(vec3(-100, eps, -100), vec3(100, eps, -100), vec3(-100, eps, 100)), 0);
	scn.world.add(sphere(vec3(-1, 0.95f, -3, 1)), 1);
	scn.world.add(sphere(vec3(1, 0.95f, -3, 1)), 2);
	//scn.world.add(voxel(vec3(0, 0.95f, 3, 1)), 3, 1);
	//scn.world.add(sphere(vec3(3, 0.95f, 0, 1)), 0);
	//scn.world.add(sphere(vec3(-3, 0.95f, 0, 1)), 0);
	
	scn.cam.T.set_P(vec3(0.0, 0.5, 0.0));
	scn.cam.T.rot();
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
	matrix T;
	vec3 TP, TA;
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
	scn1(scn);
	printf("init done!!!");

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
			if (ImGui::DragFloat("FOV", &scn.cam.fov, 0.1, 0.001, 179, "%.1f"))
			{
				moving = 1;
				scn.cam.update();
			}
			ImGui::DragFloat("Speed", &scn.cam.speed, 0.001, 0.001, 1e3f);
			moving |= ImGui::DragFloat("F-stop", &scn.cam.fstop, 0.1, 0.1, 64.f);

			if (ImGui::DragFloat("Res scale", &scale, 0.01, 0.01, 2, " % .2f", ImGuiSliderFlags_AlwaysClamp))
			{
				scn.cam.resize(width, height, scale);
				scn.cam.update();
				SDL_DestroyTexture(frame);
				frame = SDL_CreateTexture(renderer, Format, SDL_TEXTUREACCESS_STREAMING, scn.cam.w, scn.cam.h);
			}
			ImGui::DragFloat("Render Rate", &scn.opt.res_rate, 0.01, 0.01, 1, "%.2f");
			if (ImGui::DragFloat("Ray lifetime", &scn.opt.p_life, 0.01, 0.0, 1, "%.2f"))scn.opt.i_life = 1.f / scn.opt.p_life;
			ImGui::DragInt("Samples", &scn.opt.samples, 0.1, 0, 100);
			moving |= ImGui::DragInt("Bounces", &scn.opt.bounces, 0.1, 0, 100);
			moving |= ImGui::Checkbox("Light sampling", &scn.opt.li_sa); ImGui::SameLine();
			moving |= ImGui::Checkbox("Sun sampling", &scn.opt.sun_sa);

#if DEBUG
			
			moving |= ImGui::Checkbox("Debug Aten", &scn.opt.dbg_at); ImGui::SameLine();
			moving |= ImGui::Checkbox("Debug N", &scn.opt.dbg_n);
			moving |= ImGui::Checkbox("Debug UV", &scn.opt.dbg_uv);	ImGui::SameLine();
			moving |= ImGui::Checkbox("Debug t", &scn.opt.dbg_t);
			moving |= ImGui::Checkbox("Normal maps", &use_normal_maps);
			moving |= ImGui::Checkbox("Recursion", &scn.opt.recur);
			moving |= ImGui::Checkbox("Perf mode", &scn.opt.p_mode);
			static bool denoise = 1;
			ImGui::Checkbox("Denoise", &denoise);// ImGui::SameLine();
			//ImGui::Checkbox("Median", &Median);
			moving |= !denoise;
#endif
			ImGui::Text("%.2f ms %.1f FPS, %.0f", 1000.0f / smooth_fps, smooth_fps, scn.cam.CCD.t);

			ImGui::End();
			ImGui::SetNextWindowPos(ImVec2(WIDTH, HEIGHT / 2));
			ImGui::SetNextWindowSize(ImVec2(WIDTH * 16 / 12 - WIDTH, HEIGHT / 2));
			ImGui::Begin("Object properties");
			ImGui::Text("ID: %d, Type: %d", id.id, id.type);
			if(id.type!=o_bla){
			albedo& alb = scn.world.materials[scn.world.objects[id.id].get_mat()].tex;
			int type = (int)scn.world.materials[scn.world.objects[id.id].get_mat()].type;
			vec3 col = alb.get_rgb(), mer = alb.get_mer();
			moving |= ImGui::SliderInt("Mat", &type,0,5);
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