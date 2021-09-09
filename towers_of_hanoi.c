#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PRINT_TOWER_PLATES 1
#define MAX_PLATES_NUMBER 16
#define MIN_PLATES_NUMBER 1
#define ERR_CODE_NUM_FORMAT_ERR -1
#define ERR_CODE_NUM_VALUE_ERR -2
#define ERR_CODE_NO_MEM -11
#define NEW_LINE_STYLE "\r\n"
//reduce the size of stack
#define EXCHANGE_VALUE(x1, x2) ((x1) = (x1) ^ (x2), \
                                (x2) = (x1) ^ (x2), \
                                (x1) = (x1) ^ (x2))
#define T_CHILD(t, x) (t->tower[t->x].child)

typedef struct hanoi_plate
{
    int context;
    struct hanoi_plate *prev;
    struct hanoi_plate *next;
} hanoi_plate_t;

typedef struct hanoi_tower
{
    char context;
    hanoi_plate_t *child;
} hanoi_tower_t;

typedef struct hanoi_towers
{
    hanoi_tower_t tower[3];
    char src;
    char dst;
    char helper;
    unsigned int step_cnt;
} hanoi_towers_t;

void hanoi_move_plates(hanoi_towers_t *t, int plates_num);

static int input_plates_number(int *number, int min_number, int max_number);
static int init_plates_mem(int number, hanoi_towers_t *t);
static void printf_towers_plates(hanoi_towers_t *t);
static void hanoi_move_plate(hanoi_towers_t *t);

static int input_plates_number(int *number, int min_number, int max_number)
{
    int scan_result;
    printf("Input hanoi plates number(<=%d):", max_number);
    scan_result = scanf("%d", number);
    printf(NEW_LINE_STYLE);
    if (scan_result != 1)
    {
        printf("The number format error!" NEW_LINE_STYLE);
        return ERR_CODE_NUM_FORMAT_ERR;
    }
    if ((*number > max_number) || (*number < min_number))
    {
        printf("Do not support the number %d." NEW_LINE_STYLE, *number);
        return ERR_CODE_NUM_VALUE_ERR;
    }
    return 0;
}

static int init_plates_mem(int number, hanoi_towers_t *t)
{
    int index;
    hanoi_plate_t *p = calloc((size_t)number, sizeof(hanoi_plate_t));
    if (NULL == p)
    {
        printf("Don't have enough memory." NEW_LINE_STYLE);
        return ERR_CODE_NO_MEM;
    }
    memset(t, 0, sizeof(hanoi_towers_t));
    t->src = 0;
    t->dst = 2;
    t->helper = 1;
    t->tower[t->src].context = 'a';
    t->tower[t->helper].context = 'b';
    t->tower[t->dst].context = 'c';
    t->step_cnt = 0;

    t->tower[0].child = p;
    for (index = 0; index < number; index++)
    {
        p[index].context = index + 1;
        p[index].prev = &p[index - 1];
        p[index].next = &p[index + 1];
    }
    index--;
    p[0].prev = &p[index];
    p[index].next = NULL;
    return 0;
}

static void printf_towers_plates(hanoi_towers_t *t)
{
    int index;
    hanoi_tower_t *n = (hanoi_tower_t *)t;
    for (index = 0; index < sizeof(hanoi_towers_t) / sizeof(hanoi_tower_t); index++)
    {
        hanoi_plate_t *p = n[index].child;
        printf("%c:", n[index].context);
        while (p)
        {
            printf(" %d", p->context);
            p = p->next;
        }
        printf(NEW_LINE_STYLE);
    }
    printf(NEW_LINE_STYLE);
}

static void hanoi_move_plate(hanoi_towers_t *t)
{
    hanoi_plate_t *sentry;
    t->step_cnt++;
    printf("==== step %-5u ====" NEW_LINE_STYLE, t->step_cnt);
    printf("%c(%d) --> ", t->tower[t->src].context, T_CHILD(t, src)->context);
    sentry = T_CHILD(t, dst);
    T_CHILD(t, dst) = T_CHILD(t, src);

    if (T_CHILD(t, src)->next)
    {
        T_CHILD(t, src)->next->prev = T_CHILD(t, src)->prev;
    }
    T_CHILD(t, src) = T_CHILD(t, src)->next;

    T_CHILD(t, dst)->next = sentry;
    if (sentry)
    {
        T_CHILD(t, dst)->prev = sentry->prev;
        sentry->prev = T_CHILD(t, dst);
    }
    else
    {
        T_CHILD(t, dst)->prev = T_CHILD(t, dst);
    }
    printf("%c(%d)" NEW_LINE_STYLE, t->tower[t->dst].context, T_CHILD(t, dst)->context);

#if PRINT_TOWER_PLATES
    printf_towers_plates(t);
#endif
}

void hanoi_move_plates(hanoi_towers_t *t, int plates_num)
{
    if ((T_CHILD(t, src) == NULL) || (plates_num <= 0))
        return;

    //src(helper)dst -> src(dst)helper
    EXCHANGE_VALUE(t->helper, t->dst);
    hanoi_move_plates(t, plates_num - 1);

    //src(dst)helper -> src(helper)dst
    EXCHANGE_VALUE(t->helper, t->dst);
    hanoi_move_plate(t);

    //src(helper)dst -> helper(src)dst
    EXCHANGE_VALUE(t->helper, t->src);
    hanoi_move_plates(t, plates_num - 1);

    //helper(src)dst -> src(helper)dst
    EXCHANGE_VALUE(t->helper, t->src);
}

int main(void)
{
    int number;
    int ret_code;
    hanoi_towers_t towers;

    ret_code = input_plates_number(&number, MIN_PLATES_NUMBER, MAX_PLATES_NUMBER);
    if (ret_code)
        return ret_code;

    ret_code = init_plates_mem(number, &towers);
    if (ret_code)
        return ret_code;

    printf_towers_plates(&towers);

    hanoi_move_plates(&towers, number);

    return 0;
}
