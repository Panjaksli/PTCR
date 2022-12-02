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
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	SDL_PixelFormatEnum Format = SDL_PIXELFORMAT_ARGB8888;
	SDL_Texture* frame = SDL_CreateTexture(renderer, Format, SDL_TEXTUREACCESS_STREAMING, SCALE * width, SCALE * height);
	SDL_Rect out = { 0,0,WIDTH,HEIGHT };
	SDL_Event event;
	event_handler handle;
	bool running = true;

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
	ImGui_ImplSDLRenderer_Init(renderer);
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.WantCaptureMouse = 1;
	bool overlay = true;
	
	double frametime = 0;
	double target_fps = 90;
	double mov = 1;
	double mul = 1;
	double target_frametime = 1.f / target_fps;
	double smooth_fps = 0;
	obj_id id;
	scene Scene(90, SCALE * width, SCALE * height);
	float& scale = Scene.opt.res_scale;
	bool& moving = Scene.cam.moving;
	bool tap_to_focus = 0;
	int scn_n = 1;
	int node_size = 8;
	scn_load(Scene, scn_n, node_size);
	vec3 TP = Scene.sun_pos.P();
	vec3 TA = Scene.sun_pos.A;
	float fogdens = 0.01f;
	printf("init done!!!\n");
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
					matrix T;
					id = Scene.get_id(y, x, T);
					if (tap_to_focus)
						Scene.cam_manufocus(y, x);
					TP = T.P();
					TA = T.A;
				}
				if (handle.motion && handle.lmb)
					Scene.cam.rotate(0, -torad(0.9 * handle.xvec), -torad(0.9 * handle.yvec));
			}
		}
		if (handle[SDL_SCANCODE_LSHIFT])mul = 2.0;
		else mul = 1.0;
		if (handle[SDL_SCANCODE_W])Scene.cam.move_fps(mul * mov, 0, 0);
		if (handle[SDL_SCANCODE_S])Scene.cam.move_fps(-mul * mov, 0, 0);
		if (handle[SDL_SCANCODE_A])Scene.cam.move_fps(0, 0, -mul * mov);
		if (handle[SDL_SCANCODE_D])Scene.cam.move_fps(0, 0, mul * mov);
		if (handle[SDL_SCANCODE_SPACE])Scene.cam.move_fps(0, mul * mov, 0);
		if (handle[SDL_SCANCODE_LCTRL])Scene.cam.move_fps(0, -mul * mov, 0);
		if (handle[SDL_SCANCODE_F1])overlay = !overlay, handle[SDL_SCANCODE_F1] = 0;
		
		if (overlay)
		{
			ImGui_ImplSDLRenderer_NewFrame();
			ImGui_ImplSDL2_NewFrame();
			ImGui::NewFrame();
			ImGui::SetNextWindowPos(ImVec2(WIDTH, 0));
			ImGui::SetNextWindowSize(ImVec2(WIDTH * 16 / 12 - WIDTH, HEIGHT / 2));
			ImGui::Begin("Camera settings");
			ImGui::SameLine();
			if (ImGui::Button("Screenshot")) {
				save_hdr(Scene.cam.CCD.data, Scene.cam.w, Scene.cam.h,Scene.cam.CCD.spp);
			}
			if (ImGui::SliderInt("Scene", &scn_n, 1, no_scn)) {
				id = obj_id();
				moving = 1;
				node_size = 8;
				scn_load(Scene,scn_n, node_size);
				TP = Scene.sun_pos.P();
				TA = Scene.sun_pos.A;
				fogdens = log10f(-Scene.opt.ninv_fog);
			}
			ImGui::DragFloat("Speed", &Scene.cam.speed, 0.001, 0.001, 1e3f, "% .2f");
			if (ImGui::DragFloat("FOV", &Scene.cam.fov, 0.1, 0.001, 179, "%.1f"))
			{
				moving = 1;
				Scene.cam.update();
			}
			if (ImGui::Checkbox("Auto focus", &Scene.cam.autofocus))
			{
				if (Scene.cam.autofocus)tap_to_focus = 0;
				moving = 1;
			}
			ImGui::SameLine();
			if (ImGui::Checkbox("Tap to focus", &tap_to_focus))
			{
				if (tap_to_focus)Scene.cam.autofocus = 0;
			}
			if (ImGui::InputFloat("Foc dist", &Scene.cam.foc_t, 0.001, 0.001))
			{
				moving = 1;
				Scene.cam.autofocus = 0;
			}
			moving |= ImGui::DragFloat("Exposure", &Scene.cam.exposure, 0.1, 0.01, 100.f, "% .2f");
			moving |= ImGui::DragFloat("F-stop", &Scene.cam.fstop, 0.1, 0.1f, 64.f, "% .2f");
			ImGui::Checkbox("Perf mode", &Scene.opt.p_mode);
			if (ImGui::DragFloat("Res scale", &scale, 0.01, 0.1, 4, " % .2f", CLAMP))
			{
				Scene.cam.resize(width, height, scale);
				Scene.cam.update();
				SDL_DestroyTexture(frame);
				frame = SDL_CreateTexture(renderer, Format, SDL_TEXTUREACCESS_STREAMING, Scene.cam.w, Scene.cam.h);
			}
			ImGui::DragFloat("Render Rate", &Scene.opt.res_rate, 0.01, 0.01, 1, "%.2f", CLAMP);
			if (ImGui::DragFloat("Ray lifetime", &Scene.opt.p_life, 0.01, 0.01, 1.f, "%.2f", CLAMP))
				Scene.opt.i_life = 1.f / Scene.opt.p_life, moving = 1;
			ImGui::InputInt("Samples", &Scene.opt.samples, 1, 1);
			if (ImGui::DragFloat("Fog density", &fogdens, -0.01, -1, 12, "1e-%.2f", CLAMP)) {
				Scene.opt.ninv_fog = -1.f * pow(10,fogdens); moving = 1;
			}
			moving |= ImGui::InputInt("Bounces", &Scene.opt.bounces, 1, 1);
			moving |= ImGui::Checkbox("Light sampling", &Scene.opt.li_sa); ImGui::SameLine();
			moving |= ImGui::Checkbox("Sun sampling", &Scene.opt.sun_sa);
			moving |= ImGui::Checkbox("Sky", &Scene.opt.sky);

#if DEBUG

			moving |= ImGui::Checkbox("Debug Aten", &Scene.opt.dbg_at); ImGui::SameLine();
			moving |= ImGui::Checkbox("Debug N", &Scene.opt.dbg_n);
			moving |= ImGui::Checkbox("Debug UV", &Scene.opt.dbg_uv);	ImGui::SameLine();
			moving |= ImGui::Checkbox("Debug t", &Scene.opt.dbg_t);
			moving |= ImGui::Checkbox("Normal maps", &use_normal_maps); ImGui::SameLine();
			moving |= ImGui::Checkbox("Fog", &Scene.opt.en_fog);
			moving |= ImGui::Checkbox("BVH", &Scene.world.en_bvh); ImGui::SameLine();
			static bool denoise = 1;
			ImGui::Checkbox("Denoise", &denoise);// ImGui::SameLine();
			moving |= !denoise;
#endif
			ImGui::Text("%.2f ms %.1f FPS,SPP %.1fk", 1000.0f / smooth_fps, smooth_fps, Scene.cam.CCD.spp * 0.001f);
			ImGui::End();

			ImGui::SetNextWindowPos(ImVec2(WIDTH, HEIGHT / 2));
			ImGui::SetNextWindowSize(ImVec2(WIDTH * 16 / 12 - WIDTH, HEIGHT / 2));

			ImGui::Begin("Object properties");
			
			if (ImGui::InputInt("BVH Nodes", &node_size, 1, 1)) {
				node_size = node_size <= 2 ? 2 : node_size;
				node_size = node_size >= 64 ? 64 : node_size;
				Scene.world.rebuild_bvh(1, node_size);
			}
			ImGui::Text("ID: %d, Type: %d", id.id, id.type);
			if (id.type != o_bla) {
				const mesh_var& obb = id.bvh ? Scene.world.objects[id.id] : Scene.world.skp_bvh[id.id];
				albedo& alb = Scene.world.materials[obb.get_mat()].tex;
				int type = (int)Scene.world.materials[obb.get_mat()].type;
				vec3 col = alb.get_rgb(), mer = alb.get_mer();
				moving |= ImGui::SliderInt("Mat", &type, 0, material::mat_cnt - 1);
				moving |= ImGui::InputFloat("Rep", &alb.get_rep(), 1, 10);
				moving |= ImGui::ColorEdit4("Col", col._xyz, ImGuiColorEditFlags_Float);
				moving |= ImGui::ColorEdit3("Mer", mer._xyz, ImGuiColorEditFlags_Float);
				moving |= ImGui::DragFloat("Ir", &alb.get_ir(),0.01f, 1.f, 4.f);
				Scene.world.materials[obb.get_mat()].type = (mat_enum)type;
				alb.set_rgb(col);
				alb.set_mer(mer);
			}
			if (ImGui::DragFloat3("Pos", TP._xyz, 0.01))
			{
				Scene.set_trans(id, matrix(TP,TA), node_size);
				moving = 1;
			}
			if (ImGui::DragFloat3("Rot", TA._xyz, 0.01))
			{
				Scene.set_trans(id, matrix(TP, TA), node_size);
				moving = 1;
			}
			ImGui::End();
			ImGui::Render();
			ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
		}

		uint* disp; int pitch;
		SDL_LockTexture(frame, 0, (void**)&disp, &pitch);
		Scene.Render(disp, pitch / 4);
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