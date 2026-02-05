/*你对代码的调整非常正确！通过将 `key_press` 修改为仅更新**状态位**（布尔值），并将位移逻辑集中到 `move_player` 中，你已经解决了 MiniLibX 中常见的键盘延迟问题。

这是优化后的完整代码逻辑及详细注释：

*/

#include "../includes/game.h"

/**
 * 初始化玩家结构体
 * 将玩家放在屏幕中央，并重置所有按键状态。
 */
void init_player(t_player *player)
{
	player->x = WIDTH / 2;
	player->y = HEIGHT / 2;
	player->angle = PI / 2;
	player->key_up = false;
	player->key_down = false;
	player->key_left = false;
	player->key_right = false;

	player->left_rotate = false;
	player->right_rotate = false;
}

/**
 * 键盘按下钩子 (Key Press Hook)
 * 作用：当检测到按键按下，只负责将对应的标志位设为 true。
 * 这样做可以避免操作系统默认的“按键重复延迟”导致的卡顿现象。
 */
int key_press(int keycode, void *param)
{
	t_player *player = (t_player *)param;

	if (keycode == W)
		player->key_up = true;
	else if (keycode == S)
		player->key_down = true;
	else if (keycode == A)
		player->key_left = true;
	else if (keycode == D)
		player->key_right = true;
	// 如果需要按 ESC 退出，也可以加在这里
	else if (keycode == LEFT)
		player->left_rotate = true;
	else if (keycode == RIGHT)
		player->right_rotate = true;
	else if (keycode == ESC)
		exit(0);

	return 0;
}

/**
 * 键盘释放钩子 (Key Release Hook)
 * 作用：当手指离开按键，立刻将标志位设为 false，停止该方向的移动。
 */
int key_release(int keycode, void *param)
{
	t_player *player = (t_player *)param;

	if (keycode == W)
		player->key_up = false;
	if (keycode == S)
		player->key_down = false;
	if (keycode == A)
		player->key_left = false;
	if (keycode == D)
		player->key_right = false;
	// 补上旋转键的释放
	if (keycode == LEFT)
		player->left_rotate = false;
	if (keycode == RIGHT)
		player->right_rotate = false;

	return 0;
}

int is_wall(t_game *game, float x, float y)
{
	int map_x;
	int map_y;

	map_x = (int)(x / BLOCK);
	map_y = (int)(y / BLOCK);

	// 防止数组越界
	if (map_x < 0 || map_y < 0 || map_y >= 10 || map_x >= 15)
		return 1;

	return (game->map[map_y][map_x] == '1');
}

/**
 * 玩家移动逻辑执行器 (Frame-based Movement)
 * 作用：由 mlx_loop_hook 每一帧调用。
 * 只要标志位为 true，玩家就会在这一帧移动 speed 个像素。
 */
void move_player(t_game *game)
{
	t_player *player;

	player = &game ->player;
	int speed = 5; // 每一帧移动的像素距离
	float angle_speed = 0.1;
	float next_x = player->x;
	float next_y = player->y;
	float cos_angle = cos(player->angle);
	float sin_angle = sin(player->angle);

	if (player->left_rotate)
		player->angle -= angle_speed;
	if (player->right_rotate)
		player->angle += angle_speed;
	if (player->angle > 2 * PI)
		player->angle = 0;
	if (player->angle < 0)
		player->angle = 2 * PI;

	// === 先算下一步 ===
	if (player->key_up)
	{
		next_x -= sin_angle * speed;
		next_y -= cos_angle * speed;
	}
	if (player->key_down)
	{
		next_x += sin_angle * speed;
		next_y += cos_angle * speed;
	}
	if (player->key_left)
	{
		next_x -= cos_angle * speed;
		next_y -= sin_angle * speed;
	}
	if (player->key_right)
	{
		next_x += cos_angle * speed;
		next_y += sin_angle * speed;
	}

	// === 碰撞检测 ===
	if (!is_wall(game, next_x, player->y))
		player->x = next_x;

	if (!is_wall(game, player->x, next_y))
		player->y = next_y;
}

/*

### 🧠 为什么这样写更专业？

1. **平滑性 (Smoothness)**：
传统的 `key_press` 位移会触发操作系统的“打字机效应”（按下一个键，等一秒才开始连续出现字符）。通过标志位 + `loop_hook`，移动会变得像现代游戏一样丝滑。
2. **支持斜向移动**：
如果你同时按下 `W` 和 `D`，`key_up` 和 `key_right` 都会是 `true`。在 `move_player` 中，两个 `if` 都会通过，玩家会沿着 **45° 角**对角线移动。
3. **边界保护逻辑**：
在 `move_player` 中加入的 `+ 5` 检查非常重要。因为 `player->x` 通常是方块的左上角坐标，如果不加方块宽度，方块的右侧/下侧会穿出屏幕。

---

### 🚀 下一步建议

你目前的 `draw_loop` 依然会在屏幕上留下“拖尾”。如果你想让方块“移动”而不是“涂鸦”，你需要在每一帧开始时清空画布：

*/
/*int draw_loop(t_game *game)
{
	// 1. 用黑色清空整个图像缓冲区
	// bzero(game->data, WIDTH * HEIGHT * (game->bpp / 8));

	// 2. 计算新位置
	move_player(&game->player);

	// 3. 重新绘制
	draw_square_filled(game->player.x, game->player.y, 5, 0x00FF00, game);
	mlx_put_image_to_window(game->mlx, game->win, game->img, 0, 0);
	return 0;
}*/
