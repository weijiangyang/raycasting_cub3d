/*这段代码是一个基于 **MiniLibX (MLX)** 图形库的简单 2D 绘图框架，通常用于 42 学校的 **Cub3D** 或 **So_long** 项目。它实现了初始化窗口、像素操作以及简单的玩家移动（由于不清理上一帧，会留下移动轨迹）。

下面是详细的中文注释：
*/
#include "../includes/game.h"

char **get_map(void)
{
	char **map = malloc(sizeof(char *) * 11);
	map[0] = "111111111111111";
	map[1] = "100000000000001";
	map[2] = "100000000000001";
	map[3] = "100000100000001";
	map[4] = "100000001000001";
	map[5] = "100000000000001";
	map[6] = "100001000000001";
	map[7] = "100000010000001";
	map[8] = "100000000000001";
	map[9] = "111111111111111";
	map[10] = NULL;
	return (map);
}
/**
 * 初始化游戏环境
 * 设置 MLX 实例、窗口、图像缓冲区以及玩家初始位置
 */
void init_game(t_game *game)
{
	// 1. 初始化 MLX 连接
	game->mlx = mlx_init();
	if (!game->mlx)
		exit(1);

	// 2. 创建一个新窗口 (宽度, 高度, 标题)
	game->win = mlx_new_window(game->mlx, WIDTH, HEIGHT, "Raycaster");
	if (!game->win)
		exit(1);

	// 3. 创建一个图像缓冲区 (Image)，直接操作像素比直接画在窗口上快得多
	game->img = mlx_new_image(game->mlx, WIDTH, HEIGHT);
	if (!game->img)
		exit(1);

	// 4. 获取图像的原始数据地址
	// bpp: 每个像素占用的位数 (bits per pixel, 通常 32)
	// size_line: 图像一行数据占用的字节数
	// endian: 大小端序
	game->data = mlx_get_data_addr(game->img, &game->bpp, &game->size_line, &game->endian);

	// 5. 初始化玩家位置到屏幕中心
	game->player.x = WIDTH / 2;
	game->player.y = HEIGHT / 2;
	game->map = get_map();
}

/**
 * 向图像缓冲区中的特定坐标 (x, y) 写入颜色
 * 这是所有绘图功能的基础
 */
void put_pixel(int x, int y, int color, t_game *game)
{
	// 边界检查：防止写到内存外面导致段错误 (Segmentation Fault)
	if (x >= WIDTH || y >= HEIGHT || x < 0 || y < 0)
		return;

	// 计算当前像素在 data 数组中的起始字节偏移量
	// y * 一行的字节数 + x * 每个像素占用的字节数
	int index = y * game->size_line + x * (game->bpp / 8);

	// MLX 默认通常是小端序存储 (B, G, R, A)
	game->data[index] = color & 0xFF;			  // 蓝色 (Blue)
	game->data[index + 1] = (color >> 8) & 0xFF;  // 绿色 (Green)
	game->data[index + 2] = (color >> 16) & 0xFF; // 红色 (Red)
	game->data[index + 3] = 0;					  // 透明度 (Alpha/Transparency)
}

/**
 * 绘制一个填充颜色的正方形
 */
void draw_square_filled(int x, int y, int size, int color, t_game *game)
{
	int i = 0;
	while (i < size)
	{
		put_pixel(x + i, y, color, game);
		i++;
	}
	int j = 0;
	while (j < size)
	{
		put_pixel(x + i - 1, y + j, color, game);
		j++;
	}

	int k = 0;
	while (k < size)
	{
		put_pixel(x + i - 1 - k, y + j - 1, color, game);
		k++;
	}

	int l = 0;
	while (l < size)
	{
		put_pixel(x + i - 1 - k + 1, y + j - 1 - l, color, game);
		l++;
	}
}

void draw_map(t_game *game)
{
	char **map = game->map;
	int color = 0x0000FF;
	for (int y = 0; map[y]; y++)
	{
		for (int x = 0; map[y][x]; x++)
		{
			if (map[y][x] == '1')
				draw_square_filled(x * BLOCK, y * BLOCK, BLOCK, color, game);
		}
	}
}

int clear_img(t_game *game)
{
	int y;
	y = 0;
	while (y < HEIGHT)
	{
		int x = 0;
		while (x < WIDTH)
		{
			put_pixel(x, y, 0x00, game);
			x++;
		}
		y++;
	}
	return 0;
}

bool touch(float px, float py, t_game *game)
{
	// 1. 基础越界检查
	if (px < 0 || py < 0 || px >= WIDTH || py >= HEIGHT)
		return (true);

	int x = (int)px / BLOCK;
	int y = (int)py / BLOCK;

	// 2. 检查地图数组边界（防止 map[1000][1000] 这种错误）
	// 假设你的地图高度是 10，宽度是 15
	if (y < 0 || y >= 10 || x < 0 || x >= 15)
		return (true);

	if (game->map[y][x] == '1')
		return (true);

	return false;
}

// 绘制单条射线
void draw_line(t_player *player, t_game *game, float ray_angle)
{
    float cos_angle = cos(ray_angle);
    float sin_angle = sin(ray_angle);
    float ray_x = player->x;
    float ray_y = player->y;
    
    // 增加步进上限，防止死循环 (1000次步进通常足够覆盖屏幕)
    int max_steps = 1000; 
    while (!touch(ray_x, ray_y, game) && max_steps > 0)
    {
        put_pixel((int)ray_x, (int)ray_y, 0xFF0000, game);
        ray_x += cos_angle;
        ray_y += sin_angle;
        max_steps--;
    }
}

int draw_loop(t_game *game)
{
    t_player *player = &game->player;

    clear_img(game);
    move_player(player); // 确保 move_player 里也处理了旋转逻辑
    draw_map(game);

    // 绘制玩家小方块
    draw_square_filled(player->x, player->y, 5, 0x00FF00, game);

    // 计算 FOV (视场角)
    // PI / 3 = 60度
    float fraction = (PI / 3) / WIDTH; 
    float start_angle = player->angle - (PI / 6); // 玩家视角左侧 30 度开始
    
    int i = 0;
    while (i < WIDTH)
    {
        draw_line(player, game, start_angle);
        start_angle += fraction;
        i++;
    }

    mlx_put_image_to_window(game->mlx, game->win, game->img, 0, 0);
    return 0;
}

int main(void)
{
	t_game game;

	// 1. 初始化
	init_game(&game);

	// 2. 注册钩子 (Hooks)
	// mlx_hook(窗口, 事件类型, 事件掩码, 回调函数, 传递参数)

	// 监听键盘按下 (KeyPress - 2)
	mlx_hook(game.win, 2, 1L << 0, key_press, &game.player);
	// 监听键盘释放 (KeyRelease - 3)
	mlx_hook(game.win, 3, 1L << 1, key_release, &game.player);
	// 注册主渲染循环
	mlx_loop_hook(game.mlx, draw_loop, &game);
	// 监听窗口关闭按钮 (DestroyNotify - 17)
	mlx_hook(game.win, 17, 0, clear_img, &game);

	// 3. 启动 MLX 事件监听循环
	// 这个函数会阻塞在这里，直到程序退出
	mlx_loop(game.mlx);

	return (0);
}

/*

### 💡 核心逻辑说明

1. **像素布局 (put_pixel)**:
在 MLX 中，图像被视为一个一维字节数组。公式 `y * size_line + x * (bpp / 8)` 是将二维坐标转换为一维索引的标准做法。
2. **动画原理 (draw_loop)**:
代码使用了 `mlx_loop_hook`。如果你想消除玩家移动的“残影”，你应该在 `draw_loop` 的开头先画一个填满屏幕的黑色矩形，或者重新 `memset` 图像缓冲区。
3. **事件处理**:
`mlx_hook` 配合 `1L << 0` 等掩码是为了捕获键盘的实时状态。通常 `key_press` 会把某个布尔值设为 `true`，而 `key_release` 设为 `false`，这样在 `move_player` 里就能实现流畅的斜向移动。

**需要我帮你实现 `move_player` 或者添加背景清空功能吗？**
*/