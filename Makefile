NAME = game

CC = cc
CFLAGS = -Wall -Wextra -Werror

SRC = src/main.c
OBJ = $(SRC:.c=.o)

MLX_FLAGS = -lmlx -lXext -lX11 -lm -lz

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(OBJ) -o $(NAME) $(MLX_FLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)
