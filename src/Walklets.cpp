#include "Walklets.h"

Walklets::Walklets() {}
Walklets::~Walklets() {}

void Walklets::Train(int walk_times, int walk_steps, int window_min, int window_max, int negative_samples, double alpha, int workers){
    
    omp_set_num_threads(workers);

    cout << "Model:" << endl;
    cout << "\tWalklets" << endl;

    cout << "Parameters:" << endl;
    cout << "\twalk_times:\t\t" << walk_times << endl;
    cout << "\twalk_steps:\t\t" << walk_steps << endl;
    cout << "\twindow_min:\t\t" << window_min << endl;
    cout << "\twindow_max:\t\t" << window_max << endl;
    cout << "\tnegative_samples:\t" << negative_samples << endl;
    cout << "\talpha:\t\t\t" << alpha << endl;
    cout << "\tworkers:\t\t" << workers << endl;

    cout << "Start Training:" << endl;

    long total = walk_times*rgraph.MAX_vid;
    double alpha_min = alpha*0.0001;
    long samples = 0;

    for (int t=0; t<walk_times; ++t)
    {

        // for random keys access
        std::vector<long> random_keys(rgraph.MAX_vid);
        for (long vid = 0; vid < rgraph.MAX_vid; vid++) {
             random_keys[vid] = vid;
        }
        for (long vid = 0; vid < rgraph.MAX_vid; vid++) {
            int rdx = vid + rand() % (rgraph.MAX_vid - vid); // careful here!
            swap(random_keys[vid], random_keys[rdx]);
        }

        #pragma omp parallel for
        for (long vid=0; vid<rgraph.MAX_vid; ++vid)
        {
            double _alpha = alpha* ( 1.0 - (double)(samples)/total );
            if (_alpha < alpha_min) _alpha = alpha_min;

            vector<long> walks = rgraph.RandomWalk(vid, walk_steps);
            vector<vector<long>> train_data = rgraph.ScaleSkipGrams(walks, window_min, window_max, 0);
            rgraph.UpdatePairs(w_vertex, w_context, train_data[0], train_data[1], dim, negative_samples, _alpha);
            
            ++samples;
            if (samples % MONITOR == 0)
            {
                cout << "\tProgress:\t\t" << (double)(samples)/total * 100 << " %\r";
                cout.flush();
            }

        }

    }
    cout << "\tProgress:\t\t100.00 %\r" << endl;

}

