#ifndef PERMUTATIONS_H_
#define PERMUTATIONS_H_

#include <array>
#include <vector>
#include <iostream>
#include <complex>
#include <algorithm>
#include <numeric>
#include <cassert>

#ifdef TOOLS_HAS_BOOST_HASH_COMBINE
#include <boost/functional/hash.hpp>
#endif

template<std::size_t N> struct Permutation;

using Transposition = Permutation<2>;

template<std::size_t N>
struct Permutation
{
        typedef std::vector<std::size_t> Cycle;

        Permutation(const std::array<std::size_t, N>& in): pi(in) {
                for (std::size_t i=0; i<N; i++) {
                        pi_inv[pi[i]] = i;
                }
                
                std::array<bool,N> visited; visited.fill(false);
                while (!std::all_of(visited.cbegin(), visited.cend(), [] (bool b) {return b;})) {
                        auto start_it = std::find(visited.cbegin(), visited.cend(), false);
                        std::size_t start = std::distance(visited.cbegin(),start_it);
                        
                        visited[pi_inv[start]]=true;
                        
                        Cycle cycle;
                        cycle.push_back(start);
                        auto tmp=start;
                        for (std::size_t i=0; i<N; i++) {
                                auto next_power = pi[tmp];
                                if (next_power == start) {continue;}
                                else {
                                        cycle.push_back(next_power);
                                        visited[pi_inv[next_power]]=true;
                                        tmp = next_power;
                                }
                        }                                
                        cycles.push_back(cycle);
                }       
        }

#ifdef TOOLS_HAS_BOOST_HASH_COMBINE
        friend std::size_t hash_value(const Permutation<N>& p)
        {
                std::size_t seed = 0;
                boost::hash_combine(seed, p.pi);
                return seed;
        }
#endif
        
        bool operator== (const Permutation<N>& other) const
        {                
                return pi == other.pi;
        }
        
        static Permutation<N> Identity() {
                std::array<std::size_t, N> id;
                std::iota(id.begin(), id.end(), 0ul);
                return Permutation<N>(id);
        }

        static std::vector<Permutation<N> > all() {
                std::array<std::size_t, N> pi; std::iota(pi.begin(), pi.end(), 0ul);
                std::vector<Permutation<N> > out;
                do {
                        out.push_back(pi);
                } while (std::next_permutation(pi.begin(), pi.end()));
                return out;
        }
        
        std::string print() const {
                std::stringstream ss;
                for (const auto& c:cycles) {
                        ss << "("; for (std::size_t i=0; i<c.size(); i++) {
                                if ((i < c.size()-1)) {
                                        ss << c[i] << ",";
                                }
                                else {ss << c[i];}
                        }
                        ss << ")";
                }
                ss << std::endl;
                size_t count=0;
                for (const auto& elem:pi) {
                        ss << count << " ==> " << elem << std::endl; count++;
                }
                return ss.str();
        };
        
        std::vector<std::size_t> decompose() const {
            std::array<bool,N> visited; visited.fill(false);
            std::array<std::size_t,N> a;
            std::iota(a.begin(), a.end(), 0ul);
            
            std::vector<std::size_t> out;
            if (a == pi) {return out;} //early return if it is the identity permutation
            
            for (std::size_t i=0; i<N; i++) {
                    auto index = std::distance(a.begin(), std::find(a.begin(), a.end(), pi[i]));
                    // std::cout << "i=" << i << ", pi[i]=" << pi[i] << ", index=" << index << ", a="; for (const auto& o:a) {std::cout << o << " ";} std::cout << std::endl;
                    if (index == 0) {continue;}
                    if (i == 0) {
                            for (std::size_t j=index-1; j<index; --j) {
                                    // std::cout << "j=" << j << std::endl;
                                    std::swap(a[j],a[j+1]);
                                    out.push_back(j);
                            }
                    }
                    else {
                            for (std::size_t j=index-1; j>=i; --j) {
                                    // std::cout << "j=" << j << std::endl;
                                    std::swap(a[j],a[j+1]);
                                    out.push_back(j);
                            }
                    }
            }
            assert(a == pi and "Error when determining the swaps of a permutation.");
            return out;
        }

        std::size_t parity()
        {
                std::size_t out;
                for (const auto& c:cycles) {out += (c.size()-1)%2;}
                return out%2;
        }

        template<typename Container>
        void apply(Container& c) const {
                if (c.size() == 1) {return;}
                Container tmp(c);
                for (std::size_t i=0; i<c.size(); i++) {
                        tmp[i] = c[pi[i]];    
                }
                c=tmp;
        }

        Permutation<N> inverse() const {
                Permutation<N> out(pi_inv);
                return out;
        }
        
        std::array<std::size_t, N> pi;
        std::array<std::size_t, N> pi_inv;
        std::vector<Cycle> cycles;
};

#endif
