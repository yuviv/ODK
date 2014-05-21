#ifndef PIXELSTATS_H
#define PIXELSTATS_H

#include <vector>
#include <iostream>

typedef struct seg_stats_st {
    int n;
    float m;
    float v;
} seg_stats;

class PixelStats
{
protected:
    std::vector< std::vector<seg_stats> > stats;
    float normal_pdf(int x, float m, float s);
public:
    PixelStats() : stats(10, std::vector<seg_stats>(7,{0,0,0})) {}
    void print_stats(void) {
        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 7; j++) {
                std::cout << "n: " << stats[i][j].n << ", v: " << stats[i][j].v << ", m: " << stats[i][j].m << std::endl;
            }
        }
    }
    void add_seg(int num, int seg, int px_count);
    float get_prob(int num, int seg, int px_count);
};

#endif // PIXELSTATS_H
