CC = gcc
CFLAGS = -Wall -Wextra -O2 -pthread

# ---------- PHILOSOPHES ----------
PHILO_SRC = src/philosophes.c
PHILO_OBJ = $(PHILO_SRC:.c=.o)
PHILO_BIN = philosophes

# ---------- PROD / CONS ----------
PROD_SRC = src/prod-cons.c
PROD_OBJ = $(PROD_SRC:.c=.o)
PROD_BIN = prod-cons

# ---------- LECTEURS / ECRIVAINS ----------
RW_SRC = src/lecteurs-ecrivains.c
RW_OBJ = $(RW_SRC:.c=.o)
RW_BIN = lecteurs-ecrivains

# ---------- RÈGLE PAR DÉFAUT ----------
all: $(PHILO_BIN) $(PROD_BIN) $(RW_BIN)

# Binaire philosophes
$(PHILO_BIN): $(PHILO_OBJ)
	$(CC) $(CFLAGS) -o $@ $^

# Binaire prod-cons
$(PROD_BIN): $(PROD_OBJ)
	$(CC) $(CFLAGS) -o $@ $^

# Binaire lecteurs-écrivains
$(RW_BIN): $(RW_OBJ)
	$(CC) $(CFLAGS) -o $@ $^

# Compilation générique .c -> .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# ---------- NETTOYAGE ----------
clean:
	rm -f $(PHILO_OBJ) $(PROD_OBJ) $(RW_OBJ)

fclean: clean
	rm -f $(PHILO_BIN) $(PROD_BIN) $(RW_BIN)

re: fclean all

# ---------- COMMANDES D’EXÉCUTION ----------
run-philo:
	./philosophes $(N)

run-prod:
	./prod-cons $(P) $(C)

run-lect:
	./lecteurs-ecrivains $(W) $(R)

measure:
	bash scripts/measure.sh

plots:
	python3 scripts/plot_results.py

total: all measure plots fclean

