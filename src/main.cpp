#include <TFile.h>
#include <TTree.h>
#include <TRandom3.h>
#include <ROOT/RDataFrame.hxx>
#include <ROOT/RConfig.hxx>

#include <iostream>
#include <vector>
#include <chrono>
#include <cmath>
#include <thread>

using Clock = std::chrono::high_resolution_clock;

void generate_data(const std::string& filename, Long64_t N_EVENTS) {
    TFile file(filename.c_str(), "RECREATE");
    TTree tree("Events", "BranchBench Performance Study");

    float scalar = 0.0f;
    std::vector<float>* vec = new std::vector<float>();

    tree.Branch("scalar", &scalar);
    tree.Branch("vector", &vec);

    TRandom3 rand(42);

    for (Long64_t i = 0; i < N_EVENTS; ++i) {
        scalar = rand.Gaus();

        vec->clear();
        for (int j = 0; j < 20; ++j)
            vec->push_back(rand.Gaus());

        tree.Fill();
    }

    tree.Write();
    file.Close();
    delete vec;

    std::cout << "Generated " << N_EVENTS << " events.\n\n";
}

void benchmark_manual(const std::string& filename) {
    TFile file(filename.c_str(), "READ");
    TTree* tree = static_cast<TTree*>(file.Get("Events"));

    float scalar = 0.0f;
    std::vector<float>* vec = nullptr;

    tree->SetBranchAddress("scalar", &scalar);
    tree->SetBranchAddress("vector", &vec);

    Long64_t nentries = tree->GetEntries();

    auto start = Clock::now();

    double sum = 0.0;

    for (Long64_t i = 0; i < nentries; ++i) {
        tree->GetEntry(i);

        sum += scalar;

        for (const auto& v : *vec) {
            sum += v;

            for (int k = 0; k < 100; ++k)
                sum += std::sin(v) * std::cos(v);
        }
    }

    auto end = Clock::now();
    std::chrono::duration<double> diff = end - start;

    std::cout << "Manual time: "
              << diff.count() << " s\n";
    std::cout << "Events/sec: "
              << nentries / diff.count() << "\n\n";
}

void benchmark_rdataframe(const std::string& filename) {
    ROOT::RDataFrame df("Events", filename);

    auto start = Clock::now();

    auto sum = df.Define("total",
        [](float s, const std::vector<float>& v) {
            double tmp = s;

            for (const auto& x : v) {
                tmp += x;
                for (int k = 0; k < 100; ++k)
                    tmp += std::sin(x) * std::cos(x);
            }

            return tmp;
        },
        {"scalar", "vector"})
        .Sum("total");

    (void)*sum;

    auto end = Clock::now();
    std::chrono::duration<double> diff = end - start;

    Long64_t nentries = df.Count().GetValue();

    std::cout << "RDataFrame time: "
              << diff.count() << " s\n";
    std::cout << "Events/sec: "
              << nentries / diff.count() << "\n\n";
}

int main(int argc, char** argv) {
    Long64_t N_EVENTS = 500000;
    unsigned int nThreads = 1;  // default: single-thread

    if (argc > 1)
        N_EVENTS = std::stoll(argv[1]);

    if (argc > 2)
        nThreads = std::stoul(argv[2]);

#ifdef R__USE_IMT
    if (nThreads > 1) {
        ROOT::EnableImplicitMT(nThreads);
        std::cout << "Implicit MT enabled with "
                  << nThreads << " threads.\n\n";
    } else {
        std::cout << "Running single-threaded.\n\n";
    }
#else
    std::cout << "Running single-threaded.\n\n";
#endif

    const std::string filename = "branchbench.root";

    generate_data(filename, N_EVENTS);
    benchmark_manual(filename);
    benchmark_rdataframe(filename);

    return 0;
}
