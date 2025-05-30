
#include <stdio.h>

#include <yoyoengine.h>

#include "yoyo_c_api.h"
#include "demo_player_controller.h"
#include "demo_cutscene_manager.h"
#include "demo_ui_controller.h"
#include "demo_enemy_controller.h"
#include "demo_zone.h"
#include "demo_mainmenu_controller.h"

enum scenes {
    ZOOGIES_INTRO,
    MAINMENU,
    INTRO_CUTSCENE,
    LAB,
    END
};

enum scenes current_scene;

/*
    On scene load, check what scene we are on and init or destruct things as needed
*/
void yoyo_scene_load(char *scene_name){
    enum scenes last_scene = current_scene;

    if(strcmp(scene_name, "intro") == 0){
        current_scene = ZOOGIES_INTRO;
    }

    if(strcmp(scene_name, "mainmenu") == 0){
        if(current_scene != MAINMENU){
            init_mainmenu_controller();
        }
        current_scene = MAINMENU;
    }
    else{
        // mainmenu has no stateful cleanup
    }

    if(strcmp(scene_name, "intro_cutscene") == 0){
        if(current_scene != INTRO_CUTSCENE){
            init_cutscene("cutscenes/intro.json");
        }
        current_scene = INTRO_CUTSCENE;
    }
    else{
        if(last_scene == INTRO_CUTSCENE){
            shutdown_cutscene();
        }
    }
    
    if(strcmp(scene_name, "lab") == 0){
        if(current_scene != LAB){
            init_player_controller();
            enemy_controller_init();
            lock_by_tag("black_zone_1"); // after breaking surgery room barrac
            lock_by_tag("black_zone_2"); // after swiping blue card
            lock_by_tag("blue_room");    // getting blue card barrac
            lock_by_tag("green_room");   // swiping into green room
            lock_by_tag("lobby");        // breaking barrier into lobby
            lock_by_tag("green_hallway");
            lock_by_tag("green_card_room");
        }
        current_scene = LAB;
    }
    else{
        if(last_scene == LAB){
            shutdown_player_controller();
            enemy_controller_shutdown();
            // this is scuff but idc anymore
        }
    }

    if(strcmp(scene_name, "thanks") == 0){
        current_scene = END;
    }
}

void yoyo_handle_input(SDL_Event event){
    // we need to do our own poll for quit when we have our own input handler:
    if(event.type == SDL_QUIT){
        YG_RUNNING = false;
    }
    
    switch(current_scene){
        case LAB:
            player_input_handler(event);
    }
}

void yoyo_pre_shutdown(){
    switch(current_scene){
        case LAB:
            shutdown_player_controller();
    }
}

void yoyo_additional_render(){
    switch(current_scene){
        case MAINMENU:
            // ui_controller_additional_render();
            mainmenu_additional_render();
            break;
        case LAB:
            ui_controller_additional_render();
            // player_controller_additional_render();
            break;
    }
}

void yoyo_pre_frame(){
    switch(current_scene){
        case LAB:
            player_controller_pre_frame();
            enemy_controller_update();
            break;
        case END:
            ye_unregister_all_timers(); // dumb hack but whatever
            break;
    }
}

void yoyo_post_frame(){
    switch(current_scene){
        case MAINMENU:
            mainmenu_controller_poll();
            break;
        case INTRO_CUTSCENE:
            cutscene_poll();
            break;
        case LAB:
            player_controller_post_frame();
            break;
    }
}

void yoyo_trigger_enter(struct ye_entity *e1, struct ye_entity *e2){
    // printf("%s was entered by %s\n",e2->name, e1->name);

    player_controller_trigger_handler(e1,e2);
}

void yoyo_collision(struct ye_entity *e1, struct ye_entity *e2){
    // printf("%s collided with %s\n",e2->name, e1->name);
}

void yoyo_pre_init(){
    // would be used to init film grain... IF I HAD ONE!
}