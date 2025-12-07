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

# ---------- TAS LOCK ----------
LOCK_SRC = src/lock.c
LOCK_OBJ = $(LOCK_SRC:.c=.o)

TAS_SRC = src/test_tas.c
TAS_OBJ = $(TAS_SRC:.c=.o)
TAS_BIN = test-tas

# ---------- RÈGLE PAR DÉFAUT ----------
all: $(PHILO_BIN) $(PROD_BIN) $(RW_BIN) $(TAS_BIN)

$(PHILO_BIN): $(PHILO_OBJ)
	$(CC) $(CFLAGS) -o $@ $^

$(PROD_BIN): $(PROD_OBJ)
	$(CC) $(CFLAGS) -o $@ $^

$(RW_BIN): $(RW_OBJ)
	$(CC) $(CFLAGS) -o $@ $^

$(TAS_BIN): $(TAS_OBJ) $(LOCK_OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# ---------- NETTOYAGE ----------
clean:
	rm -f $(PHILO_OBJ) $(PROD_OBJ) $(RW_OBJ) $(LOCK_OBJ) $(TAS_OBJ)

fclean: clean
	rm -f $(PHILO_BIN) $(PROD_BIN) $(RW_BIN) $(TAS_BIN)

re: fclean all

# ---------- COMMANDES D’EXÉCUTION ----------
run-philo:
	./philosophes $(N)

run-prod:
	./prod-cons $(P) $(C)

run-lect:
	./lecteurs-ecrivains $(W) $(R)

run-tas:
	./test-tas $(N)

measure:
	bash scripts/measure.sh

plots:
	python3 scripts/plot_results.py

total: all measure plots fclean
