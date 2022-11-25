#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <imgui.h>
#include <time.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_sdlrenderer.h>
#include "scenes.h"
#include "event_handler.h"
#define WIDTH 960
#define HEIGHT 720
#define SCALE 1.f //0.
#define CLAMP ImGuiSliderFlags_AlwaysClamp
#if DEBUG
bool use_normal_maps = 1;
#endif

int main()
{
	uint width = WIDTH, height = HEIGHT;
	SDL_Init(SDL_INIT_EVERYTHING);
#if defined(_WIN32) || defined(WIN32)
	SDL_SetHint(SDL_HINT_RENDER_DRIVER, "direct3d11");
#endif
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");
	SDL_Window* window = SDL_CreateWindow("PTCR", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width * 4 / 3, height, SDL_WINDOW_SHOWN);
	SDL_SetWindowMinimumSize(window, 200, 200);
	SDL_Texture* frame;
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.WantCaptureMouse = 1;
	ImGui::StyleColorsDark();
	ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
	ImGui_ImplSDLRenderer_Init(renderer);
	SDL_PixelFormatEnum Format;
	Format = SDL_PIXELFORMAT_ARGB8888;
	frame = SDL_CreateTexture(renderer, Format, SDL_TEXTUREACCESS_STREAMING, SCALE * width, SCALE * height);
	SDL_Event event;
	event_handler handle;
	bool overlay = true;
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
	int scn_n = 1;
	int node_size = 8;
	scn_load(scn, scn_n, node_size);
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
					if (tap_to_focus)
						scn.cam_manufocus(y, x);
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
			ImGui::SameLine();
			if (ImGui::Button("Screenshot")) {
				save_hdr(scn.cam.CCD.data, scn.cam.w, scn.cam.h,scn.cam.CCD.spp);
			}
			if (ImGui::SliderInt("Scene", &scn_n, 1, no_scn)) {
				id = obj_id();
				moving = 1;
				node_size = 8;
				scn_load(scn,scn_n, node_size);
			}
			ImGui::DragFloat("Speed", &scn.cam.speed, 0.001, 0.001, 1e3f, "% .2f");
			if (ImGui::DragFloat("FOV", &scn.cam.fov, 0.1, 0.001, 179, "%.1f"))
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
			if (ImGui::Checkbox("Tap to focus", &tap_to_focus))
			{
				if (tap_to_focus)scn.cam.autofocus = 0;
			}
			if (ImGui::InputFloat("Foc dist", &scn.cam.foc_t, 0.001, 0.001))
			{
				moving = 1;
				scn.cam.autofocus = 0;
			}
			moving |= ImGui::DragFloat("Exposure", &scn.cam.exposure, 0.1, 0.01, 100.f, "% .2f", 1.1f);
			moving |= ImGui::DragFloat("F-stop", &scn.cam.fstop, 0.1, 0.1f, 64.f, "% .2f", 1.1f);

			if (ImGui::DragFloat("Res scale", &scale, 0.01, 0.1, 4, " % .2f", CLAMP))
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
			moving |= ImGui::Checkbox("Sky", &scn.opt.sky);

#if DEBUG

			moving |= ImGui::Checkbox("Debug Aten", &scn.opt.dbg_at); ImGui::SameLine();
			moving |= ImGui::Checkbox("Debug N", &scn.opt.dbg_n);
			moving |= ImGui::Checkbox("Debug UV", &scn.opt.dbg_uv);	ImGui::SameLine();
			moving |= ImGui::Checkbox("Debug t", &scn.opt.dbg_t);
			moving |= ImGui::Checkbox("Normal maps", &use_normal_maps); ImGui::SameLine();
			moving |= ImGui::Checkbox("Recursion", &scn.opt.recur);
			moving |= ImGui::Checkbox("BVH", &en_bvh); ImGui::SameLine();
			static bool denoise = 1;
			ImGui::Checkbox("Denoise", &denoise);// ImGui::SameLine();
			moving |= !denoise;
#endif
			ImGui::Text("%.2f ms %.1f FPS,SPP %.1fk", 1000.0f / smooth_fps, smooth_fps, scn.cam.CCD.spp * 0.001f);
			ImGui::End();

			ImGui::SetNextWindowPos(ImVec2(WIDTH, HEIGHT / 2));
			ImGui::SetNextWindowSize(ImVec2(WIDTH * 16 / 12 - WIDTH, HEIGHT / 2));

			ImGui::Begin("Object properties");
			
			if (ImGui::InputInt("BVH Nodes", &node_size, 1, 1)) {
				node_size = node_size <= 1 ? 1 : node_size;
				node_size = node_size >= 64 ? 64 : node_size;
				scn.world.rebuild_bvh(1, node_size);
			}
			ImGui::Text("ID: %d, Type: %d", id.id, id.type);
			if (id.type != o_bla) {
				const mesh_var& obb = id.bvh ? scn.world.objects[id.id] : scn.world.skp_bvh[id.id];
				albedo& alb = scn.world.materials[obb.get_mat()].tex;
				int type = (int)scn.world.materials[obb.get_mat()].type;
				vec3 col = alb.get_rgb(), mer = alb.get_mer();
				moving |= ImGui::SliderInt("Mat", &type, 0, material::mat_cnt - 1);
				moving |= ImGui::InputFloat("Rep", &alb.get_rep(), 1, 10);
				moving |= ImGui::ColorEdit4("Col", col._xyz, ImGuiColorEditFlags_Float);
				moving |= ImGui::ColorEdit3("Mer", mer._xyz, ImGuiColorEditFlags_Float);
				moving |= ImGui::DragFloat("Ir", &alb.get_ir(),0.01f, 1.f, 4.f);
				scn.world.materials[obb.get_mat()].type = (mat_enum)type;
				alb.set_rgb(col);
				alb.set_mer(mer);
			}
			if (ImGui::DragFloat3("Pos", TP._xyz, 0.01))
			{
				T.set_P(TP);
				scn.set_trans(id, T, node_size);
				moving = 1;
			}
			if (ImGui::DragFloat3("Rot", TA._xyz, 0.01))
			{
				T.set_A(TA);
				scn.set_trans(id, T, node_size);
				moving = 1;
			}
			ImGui::End();
			ImGui::Render();
			ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
		}

		uint* disp; int pitch;
		SDL_LockTexture(frame, 0, (void**)&disp, &pitch);
		scn.render(disp, pitch / 4);
		SDL_UnlockTexture(frame);
		SDL_RenderCopy(renderer, frame, NULL, &out);
		SDL_RenderPresent(renderer);

		frametime = (SDL_GetPerformanceCounter() - frametime) / SDL_GetPerformanceFrequency();
		double delay = target_frametime - frametime;
		mov = fmax(frametime, target_frametime) / target_frametime;
		smooth_fps = ImGui::GetIO().Framerate;
		SDL_Delay(fmaxf(0.0, 1000.0 * delay));
	}
	SDL_DestroyTexture(frame);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}