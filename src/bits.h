#define SET_BIT(VAL, N) (VAL |= (1U << N))
#define CLR_BIT(VAL, N) (VAL &= ~(1U << N))
#define IS_SET(VAL, N) (!!(VAL & (1U << N)))