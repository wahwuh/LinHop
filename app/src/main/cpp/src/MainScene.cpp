#include "MainScene.hpp"
#include <memory>

// colors --- I used macros because cglm don't deal with consts
#define COLOR_SELECTED Color{ 0.6f, 0.9f, 1.0f, 1.f }
#define COLOR_HIDDEN Color{ 0.5f, 0.35f, 0.6f, 1.f }
#define COLOR_IDLE Color{ 0.4f, 0.55f, 0.6f, 1.f }
#define COLOR_DISABLED Color{ 0.2f, 0.35f, 0.4f, 1.f }

#define PROLOG(e) float& pointerX = e.input.getPointerArray()[0][0]; \
    float& pointerY = e.input.getPointerArray()[0][1]; \
    float screenW = static_cast<float>(e.graphics.viewport()[0]); \
    float screenH = static_cast<float>(e.graphics.viewport()[1]);

// TODO wrap engine in namespace
using namespace linhop;

// global
float scroll = 0.f;

MainScene::MainScene(Engine& e) {

    PROLOG(e)

    srand((unsigned)time(nullptr) + 228);

    //try {
    //    this->audio_main = std::make_unique<AudioSource>("audio/a.wav", .5f);
    //    this->audio_alt = std::make_unique<AudioSource>("audio/b.wav", .5f);
    //    this->audio_bounce = std::make_unique<AudioSource>("audio/bounce.wav", 1.f);
    //    this->audio_fail_a = std::make_unique<AudioSource>("audio/fail.wav", 1.f);
    //    this->audio_fail_b = std::make_unique<AudioSource>("audio/fail2.wav", 1.f);
    //    this->audio_warning = std::make_unique<AudioSource>("audio/warning.wav", 1.f);
    //} catch (std::exception& exception) {
    //    e.log() << "Failed to load audio sources: " << exception.what();
    //}

    struct file saveDataFile = {}; if (file_load(&saveDataFile, "savedata.dat")) {
        memcpy((void*)&this->save_data, saveDataFile.data, sizeof(SaveData));
        file_unload(&saveDataFile);
    }

    // init clicks
    pointerX = last_click[0] = screenW / 2.f;
    pointerY = last_click[1] = screenH;
    this->prev_mouse_pos = {pointerX, pointerY};

    // class
    lines       = std::make_unique<Lines>();
    rand_lines  = std::make_unique<Lines>();
    sparks      = std::make_unique<Sparks>();
    lasers      = std::make_unique<Lasers>();
    ball_tail   = std::make_unique<Tail>(.7f);
    cursor_tail = std::make_unique<Tail>(.08f);
    ball        = std::make_unique<Ball>(e.graphics);

    lines->Reset(e.graphics);

    // labels
    this->label_menu_title = std::make_unique<Label>("LinHop", glm::vec2 {
        screenW / 2.f - 162.f, screenH / 2.f - 180.f
    });

    this->label_menu_title->setColor({0.6f, 0.8f, 1.0f, 1.f});

    this->label_menu_continue = std::make_unique<Label>("Continue", glm::vec2 {
        screenW / 2.f - 152.f, screenH / 2.f - 40.f
    });

    this->label_menu_start = std::make_unique<Label>("Start", glm::vec2 {
        screenW / 2.f - 98.f, screenH / 2.f + 20.f
    });

    this->label_menu_settings = std::make_unique<Label>("Settings", glm::vec2 {
        screenW / 2.f - 152.f, screenH / 2.f + 80.f
    });

    this->label_menu_exit = std::make_unique<Label>("Exit", glm::vec2 {
        screenW / 2.f - 78.f, screenH / 2.f + 140.f
    });

    this->label_menu_hint = std::make_unique<Label>("Left or right to change mode", glm::vec2 {
        screenW / 2.f - 238.f, screenH - 40.f
    });

    this->label_menu_hint->setColor({0.4f, 0.55f, 0.6f, 1.f});

    this->label_menu_mode = std::make_unique<Label>("Classic", glm::vec2 {
        screenW / 2.f - 74.f, screenH / 2.f - 110.f
    });

    this->label_endgame_restart = std::make_unique<Label>("Retry", glm::vec2 {
        screenW / 2.f + 35.f, screenH / 2.f
    });

    this->label_endgame_score = std::make_unique<Label>("Score: ", glm::vec2 {
        screenW / 2.f - 230.f, screenH / 2.f - 60.f
    });

    this->label_settings_title = std::make_unique<Label>("Settings", glm::vec2 {
        screenW / 2.f - 216.f, screenH / 2.f - 280.f
    });

    this->label_settings_fx = std::make_unique<Label>("FX: ", glm::vec2 {
        screenW / 2.f - 120.f, screenH / 2.f - 60.f
    });

    this->label_settings_music_volume = std::make_unique<Label>("Volume: ", glm::vec2 {
        screenW / 2.f - 196.f, screenH / 2.f
    });

    this->label_settings_reset_statistics = std::make_unique<Label>("Reset", glm::vec2 {
        screenW / 2.f - 90.f, screenH / 2.f + 60.f
    });

    this->label_settings_log = std::make_unique<Label>("Log", glm::vec2 {
        screenW / 2.f - 55.f, screenH / 2.f + 120.f
    });

    this->label_settings_back = std::make_unique<Label>("Back", glm::vec2 {
        screenW / 2.f - 70.f, screenH / 2.f + 280.f
    });

    this->label_game_score = std::make_unique<Label>("Score: ", glm::vec2 {
        0.f, 5.f
    });

    this->label_game_fps = std::make_unique<Label>(" fps", glm::vec2 {
        screenW - 80.f, 5.f
    });

    //try {
    //    this->audio_engine = std::make_unique<Audio>();
    //} catch (const std::exception& exception) {
    //    e.log() << "Failed to initialize audio engine: " << exception.what();
    //}
}

MainScene::~MainScene() {

    file_save("savedata.dat", (void*)&this->save_data, sizeof(SaveData));
    //audio_destroy(&this->audio_engine);
    //audio_source_unload(&this->audio_main);
    //audio_source_unload(&this->audio_alt);
    //audio_source_unload(&this->audio_bounce);
    //audio_source_unload(&this->audio_fail_a);
    //audio_source_unload(&this->audio_fail_b);
    //audio_source_unload(&this->audio_warning);
}

void MainScene::suspend(Engine& e) {

    (void)e;
    this->ball->deactivate();
    this->lines->deactivate();
    this->lasers->deactivate();
    this->sparks->deactivate();

    this->line.reset();
    this->small_text.reset();
    this->medium_text.reset();
    this->large_text.reset();

    //this->audio_engine->pauseAll();
}

void MainScene::resume(Engine& e) {

    this->line = std::make_unique<Line>();

    try {
        const char *const fontPath = "fonts/OCRAEXT.TTF";

        this->small_text = std::make_unique<Text>(fontPath, MainScene::small_text_size);
        this->medium_text = std::make_unique<Text>(fontPath, MainScene::medium_text_size);
        this->large_text = std::make_unique<Text>(fontPath, MainScene::large_text_size);

        this->ball->activate();
        this->lines->activate();
        this->lasers->activate();
        this->sparks->activate();
    } catch (const std::exception& exception) {
        e.log() << exception.what() << "\nShader error. Please reload screen";
        e.show_log();
    }

    this->pressed = false;
    this->pressed_once = false;
    //this->audio_engine->playAll();
}

bool MainScene::input(Engine& e) {

    if (e.input.isKeyHold(Input::Key::PointerMove)) {
        if (!this->onEventPointerMove(e)) {
            return false;
        }
    }
    if (e.input.isKeyDown(Input::Key::Pointer)) {
        if (!this->onEventPointerDown()) {
            return false;
        }
    }
    if (e.input.isKeyUp(Input::Key::Pointer)) {
        if (!this->onEventPointerUp(e)) {
            return false;
        }
    }

    if (e.input.isKeyDown(Input::Key::Up))
        this->onEventUp();
    if (e.input.isKeyDown(Input::Key::Down))
        this->onEventDown();
    if (e.input.isKeyDown(Input::Key::Left))
        this->onEventLeft();
    if (e.input.isKeyDown(Input::Key::Right))
        this->onEventRight();
    if (e.input.isKeyDown(Input::Key::Select))
        this->onEventSelect(e);

    if (e.input.isKeyHold(Input::Key::Ctrl)) {
        if (e.input.isKeyDown(Input::Key::A)) {
            this->suspend(e);
            this->resume(e);
            e.log() << "Scene reloaded";
        }
    } else {
		if (e.input.isKeyDown(Input::Key::Back)) {
			this->onEventBack(e);
        }
    }
    
    if (e.window->isFocused() == false) {
		this->game_state = GameState::PAUSED;
    }

    return true;
}

bool MainScene::update(Engine& engine) {

    PROLOG(engine)

	// update background color
	static float bgColorDirection = 0.005f * engine.window->getDeltaTime();
    if (background_color[0] > 0.2f || background_color[0] < 0.0f)
        bgColorDirection = -bgColorDirection;

    background_color[0] += -bgColorDirection / 2.f;
    background_color[1] += bgColorDirection / 3.f;
    background_color[2] += bgColorDirection / 2.f;

    // game logic
    switch (this->game_state) {
        case GameState::MENU:
        case GameState::ENDGAME:
        case GameState::INGAME:

            game_score = std::max(game_score, -static_cast<long>(
                ball->pos[1] - screenH / 2.f));

            ball->bounce_strength = 1.f + static_cast<float>(game_score) / ball_strength_mod;
            ball->gravity = 9.8f + static_cast<float>(game_score) / ball_gravity_mod;

            ball->move(engine.window->getDeltaTime());

            if (ball->collision(*lines, ball->prev_pos)) {
                sparks->push(ball->pos);
                //this->audio_engine->play(*this->audio_bounce);
            }

            if (ball->collision(*rand_lines, ball->prev_pos)) {
                sparks->push(ball->pos);
                //this->audio_engine->play(*this->audio_bounce);
            }

            // If ball reaches half of the screen then update scroll
            if (ball->pos[1] - screenH / 2.f - 10.f < scroll) {
                scroll += (ball->pos[1] - screenH / 2.f - 10.f - scroll) / 10.f;
            }

            // If game was over turn global scroll back
            if (game_state == GameState::ENDGAME) {
                scroll += (-scroll) / 100.f;
            }

            // If the ball is out of screen then stop the game
            if (ball->pos[0] < 0 || ball->pos[0] > screenW ||
                ball->pos[1] - scroll > screenH + ball->radius)
            {
                if (game_state == GameState::INGAME) {
                    //if (util::rand(0, 1) == 0)
                        //this->audio_engine->play(*this->audio_fail_a);
                    //else
                        //this->audio_engine->play(*this->audio_fail_b);

                    game_state = GameState::ENDGAME;
                }
            }

            // Random platforms
            if ((-scroll) - last_place > rand_lines_density) {
                if (util::rand(0, 1) <= 1) {
                    auto base_y = scroll - 80.0f;
                    auto base_x = util::rand(-(screenW/3.f), screenW);

                    struct line {
                        glm::vec2 first;
                        glm::vec2 second;
                    } new_line {
                        {base_x, base_y},
                        {base_x + (util::rand(0.f, screenW) / 2.f) - (screenW/2.f) / 4.f,
							base_y + util::rand(0.f, screenH) / 6.f}
                    };

                    if (util::dis_func(new_line.second[0] - new_line.first[0],
                                 new_line.second[1] - new_line.first[1]) > 30.0f) {
                        rand_lines->Push(new_line.second, new_line.first, false);
                    }
                }

                last_place += rand_lines_density;
            }

            // Push for tail
            if (this->save_data.fx_enabled) {

                ball_tail->push(glm::vec2{
                       ball->pos[0],
                       ball->pos[1]
                   }, glm::vec2{
                       ball->prev_pos[0],
                       ball->prev_pos[1]
                   }
                );

                if (pressed) {
                    cursor_tail->push(
                        glm::vec2{pointerX, pointerY + scroll},
                        prev_mouse_pos);
                }
            }

            // lasers
            if (game_score > 1000L) {
                if (util::rand(0, 600) == 1) {
                    //this->audio_engine->play(*this->audio_warning);

                    lasers->trigger(engine.graphics,
                            util::rand(0.0f, screenW - (screenW/3.0f)));
                }

                //if (lasers->live_time == 59)
                    //this->audio_engine->play(*this->audio_warning);
            }

            break;

        default:
            break;
    }

    // play random music
    //if (this->audio_main
    //    && this->audio_main->state != STATE_BUSY
    //    && this->audio_alt->state != STATE_BUSY)
    //{
    //   if (rand() % 2 == 0)
    //        this->audio_engine->play(*this->audio_main);
    //   else
    //        this->audio_engine->play(*this->audio_alt);
    //}

    return true;
}

void MainScene::render(Engine& e) {

    if (!this->input(e) || !this->update(e)) {
        return;
    }

    PROLOG(e)

    e.graphics.clear(background_color);

    lasers->draw(e.graphics, *this->line);

    if (lasers->live_time == 0 && !lasers->lasers.empty()) {

        // laser destroys ball
        if (ball->pos[0] > lasers->lasers.front().a[0] &&
            ball->pos[0] < lasers->lasers.back().a[0]) {

            if (game_state == GameState::INGAME)
                game_state = GameState::ENDGAME;

            //if (util::rand(0, 1) == 0)
            //    this->audio_engine->play(*this->audio_fail_a);
            //else
            //    this->audio_engine->play(*this->audio_fail_b);
        }

        sparks->push(glm::vec2{0.0f, 0.0f});

        for (int i = 0; i < static_cast<int>(screenH); i += static_cast<int>(screenH) / 10) {
            sparks->push(glm::vec2{lasers->lasers.back().a[0], static_cast<float>(i)});
            sparks->push(glm::vec2{lasers->lasers.front().a[0], static_cast<float>(i)});
        }
    }

    if (save_data.fx_enabled) {
        cursor_tail->draw(e.graphics, *this->line);
        sparks->draw(e.graphics);
		ball_tail->draw(e.graphics, *this->line);
    }

    if (game_mode == GameMode::CLASSIC)
        lines->draw(e.graphics);

    ball->draw(e.graphics);
    rand_lines->draw(e.graphics);

    // gui text
    switch (game_state) {
        case GameState::PAUSED:
        case GameState::MENU:

            this->label_menu_title->draw(e.graphics, *this->large_text);

            this->label_menu_continue
                ->setColor(menu_selected == MenuSelected::CONTINUE ? COLOR_SELECTED : COLOR_IDLE)
                .draw(e.graphics, *this->medium_text);

            this->label_menu_start
                ->setColor(menu_selected == MenuSelected::START ? COLOR_SELECTED : COLOR_IDLE)
                .draw(e.graphics, *this->medium_text);

            this->label_menu_settings
                ->setColor(menu_selected == MenuSelected::SETTINGS ? COLOR_SELECTED : COLOR_IDLE)
                .draw(e.graphics, *this->medium_text);

            this->label_menu_exit
                ->setColor(menu_selected == MenuSelected::EXIT ? COLOR_SELECTED : COLOR_IDLE)
                .draw(e.graphics, *this->medium_text);

            this->label_menu_hint->draw(e.graphics, *this->small_text);

            switch (game_mode) {
                case GameMode::CLASSIC:

                    this->label_game_score
                        ->setText("Score: " + std::to_string(save_data.max_score_classic))
                        .setColor(COLOR_IDLE)
                        .draw(e.graphics, *this->medium_text);

                    this->label_menu_mode
                        ->setText("Classic")
                        .setPos(glm::vec2{ screenW/2.f - 60.f, screenH / 2.f - 110.f })
                        .setColor(COLOR_IDLE)
                        .draw(e.graphics, *this->small_text);

                    break;

                case GameMode::HIDDEN:

                    this->label_game_score
                        ->setText("Score: " + std::to_string(save_data.max_score_classic))
                        .setColor(COLOR_IDLE)
                        .draw(e.graphics, *this->medium_text);

                    this->label_menu_mode
                        ->setText("Hidden")
                        .setPos(glm::vec2{ screenW/2.f - 52.f, screenH / 2.f - 110.f })
                        .setColor(COLOR_IDLE)
                        .draw(e.graphics, *this->small_text);

                    break;

                default:
                    break;
            }

            break;

        case GameState::SETTINGS:

            this->label_settings_title->draw(e.graphics, *this->large_text);

            this->label_settings_fx
                ->setText(CCAT("FX: ", save_data.fx_enabled, "on", "off"))
                .setColor(settings_selected == SettingsSelected::FX_ENABLED ? COLOR_SELECTED : COLOR_IDLE)
                .draw(e.graphics, *this->medium_text);

            this->label_settings_music_volume
                ->setText("Volume: " + std::to_string(static_cast<int>(save_data.music_volume_float * 100)))
                .setColor(settings_selected == SettingsSelected::MUSIC_VOLUME ? COLOR_SELECTED : COLOR_IDLE)
                .draw(e.graphics, *this->medium_text);

            this->label_settings_reset_statistics
                ->setColor(settings_selected == SettingsSelected::RESET_STATISTICS ? COLOR_SELECTED : COLOR_IDLE)
                .draw(e.graphics, *this->medium_text);
            
            this->label_settings_log
                ->setColor(settings_selected == SettingsSelected::LOG ? COLOR_SELECTED : COLOR_IDLE)
                .draw(e.graphics, *this->medium_text);

            this->label_settings_back
                ->setColor(settings_selected == SettingsSelected::BACK ? COLOR_SELECTED : COLOR_IDLE)
                .draw(e.graphics, *this->medium_text);

            break;

        case GameState::INGAME:

#if defined(__ANDROID__) || defined(ANDROID)
            if (pressed && game_mode == GameMode::CLASSIC) {
#else
            if (game_mode == GameMode::CLASSIC) {
#endif
                this->line->use();
                this->line->draw(e.graphics, &glm::vec4 {last_click[0], last_click[1] - scroll, pointerX, pointerY}[0], Color {
                    0.5f, 0.5f, 0.5f, 1.0f
                }, 5.f);
            }

#ifndef NDEBUG

//            this->labelGameFps
//                ->setText(std::to_string(static_cast<int>(1 / engine.window->getDeltaTime())) + std::string(" fps"))
//                .setColor(gameMode == GameMode::CLASSIC ? COLOR_SELECTED : COLOR_HIDDEN)
//                .draw();

#endif

            this->label_game_score
                ->setText("Score: " + std::to_string(game_score))
                .setColor(game_mode == GameMode::CLASSIC ? COLOR_SELECTED : COLOR_HIDDEN)
                .draw(e.graphics, *this->medium_text);

            break;

        case GameState::ENDGAME:

            this->label_endgame_score
                ->setText("Score: " + std::to_string(game_score))
                .setColor(game_mode == GameMode::CLASSIC ? COLOR_SELECTED : COLOR_HIDDEN)
                .draw(e.graphics, *this->medium_text);

            this->label_endgame_restart->draw(e.graphics, *this->medium_text);

            break;

        default:
            break;
    }

    // std::memcpy(this->line.color, (float[4]){1.f, 1.f, 1.f, 1.f}, sizeof(float[4]));

    // float debugLine[4] = {
    //     screenW/2.f, 0.f,
    //     screenW/2.f, screenH
    // };
    // float debugLineHor[4] = {
    //     0.f, screenH/2.f,
    //     screenW, screenH/2.f
    // };
    // drawable.draw(&this->line, debugLine);
    // drawable.draw(&this->line, debugLineHor);

    this->prev_mouse_pos = {
        pointerX,
        pointerY + scroll,
    };

    this->pressed_once = false;
    // engine.window->close(); // TODO REMOVE
}

void MainScene::reset(Engine& engine) {

    cursor_tail->reset();

    this->prev_mouse_pos = {
        static_cast<float>(engine.graphics.viewport()[0]) / 2.f,
        static_cast<float>(engine.graphics.viewport()[1])
    };

    this->last_click = this->prev_mouse_pos;

    scroll = 0.0f;

    if (game_mode == GameMode::CLASSIC)
        if (game_score > save_data.max_score_classic)
            save_data.max_score_classic = game_score;

    if (game_mode == GameMode::HIDDEN)
        if (game_score > save_data.max_score_hidden)
            save_data.max_score_hidden = game_score;

    game_score = 0L;

    game_state = GameState::INGAME;
    last_place = rand_lines_density;

    ball->reset(engine.graphics);

    rand_lines->Reset(engine.graphics);
    lines->Reset(engine.graphics);

    lines->lines.emplace_back( // First line
        glm::vec2{ 0.f, static_cast<float>(engine.graphics.viewport()[1]) },
        glm::vec2{static_cast<float>(engine.graphics.viewport()[0]), static_cast<float>(engine.graphics.viewport()[1]) },
        Color{ 1.0f, 1.0f, 1.0f, 1.0f },
        false
    );
}

bool MainScene::onEventPointerMove(Engine& engine) {

    auto& pointerPos = engine.input.getPointerArray()[0];

    // gestures
    switch (this->game_state) {

        case GameState::MENU:
        case GameState::PAUSED:
            if (this->label_menu_continue->isCollide(*this->medium_text, glm::make_vec2(pointerPos.data()))) {

                this->menu_selected = MenuSelected::CONTINUE;
                if (this->pressed_once) {
                    return this->onEventSelect(engine);
                }
            }

            if (this->label_menu_start->isCollide(*this->medium_text, glm::make_vec2(pointerPos.data()))) {

                this->menu_selected = MenuSelected::START;
                if (this->pressed_once) {
                    return this->onEventSelect(engine);
                }
            }
            if (this->label_menu_settings->isCollide(*this->medium_text, glm::make_vec2(pointerPos.data()))) {

                this->menu_selected = MenuSelected::SETTINGS;
                if (this->pressed_once) {
                    return this->onEventSelect(engine);
                }
            }

            if (this->label_menu_exit->isCollide(*this->medium_text, glm::make_vec2(pointerPos.data()))) {

                this->menu_selected = MenuSelected::EXIT;
                if (this->pressed_once) {
                    return this->onEventSelect(engine);
                }
            }

            if (this->label_menu_hint->isCollide(*this->small_text, glm::make_vec2(pointerPos.data()))) {
                if (this->pressed_once) {
                    this->label_menu_hint->setColor({
                        util::rand(.0f, 1.f), util::rand(.0f, 1.f), util::rand(.0f, 1.f), 1.f
                    });
                }
            }

            if (this->label_menu_mode->isCollide(*this->small_text, glm::make_vec2(pointerPos.data()))) {
                if (this->pressed_once) {
                    this->onEventRight();
                }
            }

            break;

        case GameState::ENDGAME:

            if (this->label_endgame_restart->isCollide(*this->medium_text, glm::make_vec2(pointerPos.data()))) {
                if (this->pressed_once)
                    return this->onEventSelect(engine);
            }
            break;

        case GameState::SETTINGS:

            if (this->label_settings_fx->isCollide(*this->medium_text, glm::make_vec2(pointerPos.data()))) {

                this->settings_selected = SettingsSelected::FX_ENABLED;
                if (this->pressed_once) {
                    return this->onEventSelect(engine);
                }
            }

            if (this->label_settings_music_volume->isCollide(*this->medium_text, glm::make_vec2(pointerPos.data()))) {

                this->settings_selected = SettingsSelected::MUSIC_VOLUME;
                if (this->pressed_once) {
                    this->onEventLeft();
                }
            }

            if (this->label_settings_reset_statistics->isCollide(*this->medium_text, glm::make_vec2(pointerPos.data()))) {

                this->settings_selected = SettingsSelected::RESET_STATISTICS;
                if (this->pressed_once) {
                    return this->onEventSelect(engine);
                }
            }

            if (this->label_settings_log->isCollide(*this->medium_text, glm::make_vec2(pointerPos.data()))) {

                this->settings_selected = SettingsSelected::LOG;
                if (this->pressed_once) {
                    return this->onEventSelect(engine);
                }
            }

            if (this->label_settings_back->isCollide(*this->medium_text, glm::make_vec2(pointerPos.data()))) {

                this->settings_selected = SettingsSelected::BACK;
                if (this->pressed_once) {
                    return this->onEventSelect(engine);
                }
            }
            break;
        default: break;
    }

    return true;
}

bool MainScene::onEventPointerDown() {
    pressed = true;
    return true;
}

bool MainScene::onEventPointerUp(Engine& engine) {

    if (this->game_state == GameState::INGAME) {
        glm::vec2 newPos = {
            engine.input.getPointerArray()[0][0], engine.input.getPointerArray()[0][1] + scroll
        };
        
        this->lines->Push(newPos, last_click);
        this->last_click = newPos;
    }

    this->pressed = false;
    this->pressed_once = true;

    return onEventPointerMove(engine);
}

bool MainScene::onEventSelect(Engine& engine) {

    switch (game_state) {

        case GameState::SETTINGS:
            switch (settings_selected) {
                case SettingsSelected::FX_ENABLED:
                    save_data.fx_enabled = save_data.fx_enabled == 0;
                    break;
                case SettingsSelected::RESET_STATISTICS:
                    save_data.max_score_classic = 0L;
                    save_data.max_score_hidden = 0L;
                    file_remove("savedata.dat");
                    break;
                case SettingsSelected::LOG:
                    game_state = GameState::PAUSED;
                    engine.show_log();
                    return false;
                case SettingsSelected::BACK:
                    game_state = GameState::PAUSED;
                    break;
                default:
                    break;
            }
            break;

        case GameState::PAUSED:
        case GameState::MENU:
            switch (menu_selected) {
                case MenuSelected::START:
                    game_state = GameState::INGAME;
                    reset(engine);
                    break;

                case MenuSelected::CONTINUE:
                    if (game_state == GameState::PAUSED)
                        game_state = GameState::INGAME;
                    break;

                case MenuSelected::SETTINGS:
                    game_state = GameState::SETTINGS;
                    break;

                case MenuSelected::EXIT: {
                    this->game_state = GameState::EXITING;
                    engine.window->close();
                    break;
                }

                default:
                    break;
            }
            break;

        case GameState::ENDGAME:
            reset(engine);
            break;

        default:
            break;
    }

    return true;
}

void MainScene::onEventBack(Engine& engine) {

    switch (game_state) {

        case GameState::MENU:
            engine.window->close();
            return;

        case GameState::SETTINGS:
            game_state = GameState::PAUSED;
            break;

        case GameState::INGAME:
            menu_selected = MenuSelected::CONTINUE;
            game_state = GameState::PAUSED;
            break;

        case GameState::PAUSED:
            game_state = GameState::INGAME;
            break;

        case GameState::ENDGAME:
            game_state = GameState::MENU;
            reset(engine);
            break;

        default:
            break;
    }
}

void MainScene::onEventUp() {

    if (game_state == GameState::MENU)
        --menu_selected;
    if (game_state == GameState::PAUSED)
        --menu_selected;
    if (game_state == GameState::SETTINGS)
        --settings_selected;
}

void MainScene::onEventLeft() {

    if (game_state == GameState::MENU) {
        --game_mode;
        cursor_tail->alpha = cursor_tail->alpha == 0.15f ? 0.4f : 0.15f;
    }

    if (game_state == GameState::SETTINGS) {
        if (settings_selected == SettingsSelected::MUSIC_VOLUME) {

            if (save_data.music_volume_float > 0.0f) {
                save_data.music_volume_float -= 0.1f;
            } else {
                save_data.music_volume_float = 1.f;
            }

            // this->audio_engine->master_vol = std::min(save_data.music_volume_float, 1.f);
        }
    }
}

void MainScene::onEventDown() {

    if (game_state == GameState::MENU)
        ++menu_selected;
    if (game_state == GameState::PAUSED)
        ++menu_selected;
    if (game_state == GameState::SETTINGS)
        ++settings_selected;
}

void MainScene::onEventRight() {

    if (game_state == GameState::MENU) {
        ++game_mode;
        cursor_tail->alpha = cursor_tail->alpha == 0.08f ? 0.4f : 0.08f;
    }

    if (game_state == GameState::SETTINGS) {

        if (settings_selected == SettingsSelected::MUSIC_VOLUME) {

            if (save_data.music_volume_float <= 1.0f) {
                save_data.music_volume_float += 0.1f;
            } else {
                save_data.music_volume_float = 0.f;
            }

            //this->audio_engine.master_vol = std::min(save_data.music_volume_float, 1.f);
        }
    }
}
