#include "../includes/game.h"

void init_player(t_player *player)
{
	player->x= WIDTH /2;
	player->y = HEIGHT/2;
	player->key_up = false;
	player->key_down= false;
	player->key_left = false;
	player->key_right = false;
}

int key_press(int keycode, void *param)
{
    t_player *player = param;

    int speed = 5;

    if (keycode == W && player->y - speed >= 0)
        player->y -= speed;
    if (keycode == S && player->y + speed + 5 < HEIGHT)
        player->y += speed;
    if (keycode == A && player->x - speed >= 0)
        player->x -= speed;
    if (keycode == D && player->x + speed + 5 < WIDTH)
        player->x += speed;

    return 0;
}


int key_release(int keycode, void *param)
{
    t_player *player = (t_player *)param;

    if (keycode == W) player->key_up = false;
    if (keycode == S) player->key_down = false;
    if (keycode == A) player->key_left = false;
    if (keycode == D) player->key_right = false;

    return 0;
}


void move_player(t_player *player)
{
    int speed = 5;

    if (player->key_up && player->y - speed >= 0)
        player->y -= speed;
    if (player->key_down && player->y + speed + 5 < HEIGHT) // 5 = 方块大小
        player->y += speed;
    if (player->key_left && player->x - speed >= 0)
        player->x -= speed;
    if (player->key_right && player->x + speed + 5 < WIDTH)
        player->x += speed;
}
