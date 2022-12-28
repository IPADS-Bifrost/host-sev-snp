#include <linux/kernel.h>
#include <linux/module.h>

static int cmd = 0;
static int arg = 0;
#define VCPU_NR (120)
#define EXIT_REASON_MAX (0x405)
module_param(cmd, int, S_IRUGO);
module_param(arg, int, S_IRUGO);

extern atomic64_t vmexit_cnt[][EXIT_REASON_MAX];
extern atomic64_t vmexit_cycle[][EXIT_REASON_MAX];
extern atomic_t vmexit_first[];
extern bool vmexit_record_en;

extern uint64_t breakdown_st;

static int __init breakdown_init(void)
{

    int i, j;
    uint64_t en, tot;

        // pr_info("%s", __func__);
        switch (cmd)
        {
        case 0:
                pr_info("vmexit_record_en set to %s\n", 
                                arg == 1?"true":"false");
                breakdown_st = rdtsc_ordered();
                vmexit_record_en = (arg == 1);

                break;
        case 1:
                
                break;
        case 2:
                vmexit_record_en = 0;
                for (i = 0; i < VCPU_NR; i++) {
                        for (j = 0; j < EXIT_REASON_MAX; j++) {
                                atomic64_set(&vmexit_cnt[i][j], 0);
				atomic64_set(&vmexit_cycle[i][j], 0);
                        }
                        atomic_set(&vmexit_first[i], 1);
                }
                pr_info("clear all stat data");
                // vmexit_record_en = 1;
                // st = rdtsc_ordered();
                break;
        case 3:
                if (vmexit_record_en)
                        en = rdtsc_ordered();
                vmexit_record_en = 0;
                tot = en - breakdown_st;
                pr_info("Total %lld cycles\n", en - breakdown_st);
                for (i = 0; i < VCPU_NR; i++) {
                        for (j = 0; j < EXIT_REASON_MAX; j++) {
                                uint64_t cnt, cycle, cycle1;
                                cnt = atomic64_read(&vmexit_cnt[i][j]);
                                cycle = atomic64_read(&vmexit_cycle[i][j]);
                                cycle1 = cycle + cnt * 9500;
                                cycle = cycle + cnt * 1400;
                                if (cnt) {
                                        pr_info("vcpu %d reason 0x%x cnt %lld cycle %lld percent %lld, cycle1 %lld, percent %lld\n",
                                        i, j, cnt, cycle, 100*cycle/tot, cycle1, 100*cycle1/tot);
                                }
                        }
                }
                break;
        default:
                pr_err("unexpected cmd:%d", cmd);
        }
        return 0;
}

static void __exit breakdown_exit(void)
{
        // pr_info("%s", __func__);
}

module_init(breakdown_init)
module_exit(breakdown_exit)
MODULE_LICENSE("GPL");