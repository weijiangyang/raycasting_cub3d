#include "../includes/game.h"

void put_pixel(int x,int y, int color, t_game *game)
{
	if (x >= WIDTH || y >= HEIGHT || x< 0 || y < 0)
		return;
	int index = y * game->size_line + x * (game->bpp / 8);
	game->data[index] = color & 0xFF;
	game->data[index + 1] = (color >> 8) & 0xFF;
	game->data[index + 2] = (color >> 16) & 0xFF;

}


void draw_square_filled(int x, int y, int size, int color, t_game *game)
{
    for (int i = 0; i < size; i++)       // 水平方向
    {
        for (int j = 0; j < size; j++)   // 垂直方向
        {
            put_pixel(x + i, y + j, color, game);
        }
    }
}

void init_game(t_game *game)
{
	
	game->mlx = mlx_init();
	if (!game->mlx)
		exit(1);

	game->win = mlx_new_window(game->mlx, WIDTH, HEIGHT, "Raycaster");
	if (!game->win)
		exit(1);
	game->img = mlx_new_image(game->mlx, WIDTH, HEIGHT);
	if (!game->img)
		exit(1);
	 // 获取像素数据指针
    game->data = mlx_get_data_addr(game->img,
                                   &game->bpp,
                                   &game->size_line,
                                   &game->endian);

    // 初始化 player 坐标
    game->player.x = WIDTH / 2;
    game->player.y = HEIGHT /2;


	
}

int draw_loop(t_game *game)
{
    // 不清空画布
    move_player(&game->player);
    draw_square_filled(game->player.x, game->player.y, 5, 0x00FF00, game);
    mlx_put_image_to_window(game->mlx, game->win, game->img, 0, 0);
    return 0;
}


int main(void)
{
	t_game game;

	init_game(&game);
	mlx_hook(game.win, 2, 1L<<0, key_press, &game.player);
	mlx_hook(game.win, 3, 1L<<1, key_release, &game.player);
	mlx_loop_hook(game.mlx, draw_loop, &game);
	mlx_loop(game.mlx);   // ⭐⭐⭐ 关键

	return (0);
}