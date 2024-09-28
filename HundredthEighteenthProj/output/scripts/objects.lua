objects = {}
setting = {
    type = {
        player = 0;
        ball = 1;
        projectile1 = 2;
        wall = 3;
        ground = 4;
        wall_horizontal = 5;
        wall_horizontal_top = 6;
        wall_horizontal_leftend = 7;
        wall_horizontal_leftend_top = 8;
        wall_horizontal_rightend = 9;
        wall_horizontal_rightend_top = 10;
        wall_left_up_corner = 11;
        wall_left_up_corner_top = 12;
        wall_right_up_corner = 13;
        wall_right_up_corner_top = 14;
        wall_left_down_corner = 15;
        wall_left_down_corner_bot = 16;
        wall_right_down_corner = 17;
        wall_right_down_corner_bot = 18;
        wall_vertical_left = 19;
        wall_vertical_left_bot = 20;
        wall_vertical_left_top = 21;
        wall_vertical_right = 22;
        wall_vertical_right_bot = 23;
        wall_vertical_right_top = 24;
        skeleton_king = 25;
        skeleton = 26;
    };
    type_max = 27;
    action = {
        move_down = 0;
        move_left = 1;
        move_up = 2;
        move_right = 3;
        attack = 4;
        special = 5;
    };
    action_type_max = 6;
    animation = {
        idle = 0;
        walk = 1;
        attack = 2;
        death = 3
    };
    collide_mode = {
        circle_mode = 0;
        rect_mode = 1;
        convex_mode = 2;
    };
    animation_max = 4;
    repulsion_rate = 50;
    speed_rate = 300;
    projectile_speed = 1000;
    tile_size = 128;
}
dynobjlist = {
    player = {
        type = setting.type.player;
        hp = 100;
        damage = 0;
        ai_class = "nope";
        scale = {1, 1};
        boundybox = {
            {50, 90};
            {100, 90};
            {100, 159};
            {50, 159}
        };
        interact_radius = 500;
        projectile_pos = {136, 104};
        animation = {
            idle = "animation_player_idle";
            walk = "animation_player_walk";
            attack = "animation_player_attack"
        }
    };
    ball = {
        type = setting.type.ball;
        hp = 100;
        damage = 0;
        ai_class = "nope";
        scale = {1, 1};
        boundybox = {
            {0, 0};
            {300, 0};
            {300, 300};
            {0, 300}
        };
        interact_radius = 500;
        animation = {
            idle = "animation_ball"
        }
    };
    projectile1 = {
        type = setting.type.projectile1;
        hp = 1;
        damage = 50;
        ai_class = "nope";
        scale = {1, 1};
        boundybox = {
            {150, 150};
            {175, 150};
            {175, 175};
            {150, 175}
        };
        interact_radius = 0;
        animation = {
            idle = "animation_projectile1";
            death = "animation_projectile1_blast"
        }
    };
    skeleton_king = {
        type = setting.type.skeleton_king;
        hp = 100;
        damage = 0;
        ai_class = "ai_enemy";
        scale = {1, 1};
        boundybox = {
            {20, 80};
            {290, 80};
            {290, 280};
            {20, 280}
        };
        interact_radius = 500;
        animation = {
            idle = "animation_skeleton_king_idle";
            walk = "animation_skeleton_king_walk";
            death = "animation_skeleton_king_death"
        }
    };
    skeleton = {
        type = setting.type.skeleton;
        hp = 50;
        damage = 0;
        ai_class = "ai_enemy";
        scale = {1, 1};
        boundybox = {
            {60, 85};
            {110, 85};
            {110, 159};
            {60, 159}
        };
        interact_radius = 500;
        animation = {
            idle = "animation_skeleton_idle";
            walk = "animation_skeleton_walk";
            death = "animation_skeleton_death"
        }
    }
}
stobjlist = {
    wall = {
        type = setting.type.wall;
        scale = {2, 2};
        boundybox = {
            {0, 0};
            {64, 0};
            {64, 64};
            {0, 64}
        };
        animation = {
            idle = "animation_wall"
        }
    };
    wall_right_down_corner = {
        type = setting.type.wall_right_down_corner;
        scale = {4, 4};
        boundybox = {
            {0, 0};
            {16, 0};
            {16, 16};
            {0, 16};
        };
        animation = {
            idle = "animation_wall_right_down_corner";
        }
    };
    wall_left_down_corner = {
        type = setting.type.wall_left_down_corner;
        scale = {4, 4};
        boundybox = {
            {0, 0};
            {16, 0};
            {16, 16};
            {0, 16};
        };
        animation = {
            idle = "animation_wall_left_down_corner";
        }
    };
    wall_right_up_corner = {
        type = setting.type.wall_right_up_corner;
        scale = {4, 4};
        boundybox = {
            {11, 0};
            {16, 0};
            {16, 16};
            {11, 16};
        };
        animation = {
            idle = "animation_wall_right_up_corner";
        }
    };
    wall_right_up_corner_top = {
        type = setting.type.wall_right_up_corner_top;
        scale = {4, 4};
        boundybox = {
            {0, 12};
            {16, 12};
            {16, 16};
            {0, 16};
        };
        animation = {
            idle = "animation_wall_right_up_corner_top";
        }
    };
    wall_left_up_corner = {
        type = setting.type.wall_left_up_corner;
        scale = {4, 4};
        boundybox = {
            {0, 0};
            {5, 0};
            {5, 16};
            {0, 16};
        };
        animation = {
            idle = "animation_wall_left_up_corner";
        }
    };
    wall_left_up_corner_top = {
        type = setting.type.wall_left_up_corner_top;
        scale = {4, 4};
        boundybox = {
            {0, 12};
            {16, 12};
            {16, 16};
            {0, 16};
        };
        animation = {
            idle = "animation_wall_left_up_corner_top";
        }
    };
    wall_vertical_left = {
        type = setting.type.wall_vertical_left;
        scale = {4, 4};
        boundybox = {
            {0, 0};
            {5, 0};
            {5, 16};
            {0, 16};
        };
        animation = {
            idle = "animation_wall_vertical_left";
        }
    };
    wall_vertical_right = {
        type = setting.type.wall_vertical_right;
        scale = {4, 4};
        boundybox = {
            {11, 0};
            {16, 0};
            {16, 16};
            {11, 16};
        };
        animation = {
            idle = "animation_wall_vertical_right";
        }
    };
    wall_horizontal_top = {
        type = setting.type.wall_horizontal_top;
        scale = {4, 4};
        boundybox = {
            {0, 12};
            {16, 12};
            {16, 16};
            {0, 16};
        };
        animation = {
            idle = "animation_wall_horizontal_top";
        }
    };
    wall_horizontal_leftend_top = {
        type = setting.type.wall_horizontal_leftend_top;
        scale = {4, 4};
        boundybox = {
            {0, 12};
            {16, 12};
            {16, 16};
            {0, 16};
        };
        animation = {
            idle = "animation_wall_horizontal_leftend_top";
        }
    };
    wall_horizontal_rightend_top = {
        type = setting.type.wall_horizontal_rightend_top;
        scale = {4, 4};
        boundybox = {
            {0, 12};
            {16, 12};
            {16, 16};
            {0, 16};
        };
        animation = {
            idle = "animation_wall_horizontal_rightend_top";
        }
    };
}
spritelist = {
    ground = {
        type = setting.type.ground;
        texture = "texture_tiles";
        rect = {16, 64, 16, 16};
        scale = {4, 4}
    };
    wall_horizontal = {
        type = setting.type.wall_horizontal;
        texture = "texture_tiles";
        rect = {32, 16, 16, 16};
        scale = {4, 4}
    };
    wall_horizontal_leftend = {
        type = setting.type.wall_horizontal_leftend;
        texture = "texture_tiles";
        rect = {16, 16, 16, 16};
        scale = {4, 4}
    };
    wall_horizontal_rightend = {
        type = setting.type.wall_horizontal_rightend;
        texture = "texture_tiles";
        rect = {48, 16, 16, 16};
        scale = {4, 4}
    };
    wall_vertical_left_bot = {
        type = setting.type.wall_vertical_left_bot;
        texture = "texture_tiles";
        rect = {16, 144, 16, 16};
        scale = {4, 4}
    };
    wall_vertical_left_top = {
        type = setting.type.wall_vertical_left_top;
        texture = "texture_tiles";
        rect = {16, 112, 16, 16};
        scale = {4, 4}
    };
    wall_vertical_right_bot = {
        type = setting.type.wall_vertical_right_bot;
        texture = "texture_tiles";
        rect = {0, 144, 16, 16};
        scale = {4, 4}
    };
    wall_vertical_right_top = {
        type = setting.type.wall_vertical_right_top;
        texture = "texture_tiles";
        rect = {0, 112, 16, 16};
        scale = {4, 4}
    };
    wall_left_down_corner_bot = {
        type = setting.type.wall_left_down_corner_bot;
        texture = "texture_tiles";
        rect = {32, 160, 16, 16};
        scale = {4, 4}
    };
    wall_right_down_corner_bot = {
        type = setting.type.wall_right_down_corner_bot;
        texture = "texture_tiles";
        rect = {48, 160, 16, 16};
        scale = {4, 4}
    }
}
return objects