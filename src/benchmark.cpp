/*
  Stockfish, a UCI chess playing engine derived from Glaurung 2.1
  Copyright (C) 2004-2008 Tord Romstad (Glaurung author)
  Copyright (C) 2008-2015 Marco Costalba, Joona Kiiski, Tord Romstad
  Copyright (C) 2015-2017 Marco Costalba, Joona Kiiski, Gary Linscott, Tord Romstad

  Stockfish is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Stockfish is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <fstream>
#include <iostream>
#include <istream>
#include <vector>

#include "position.h"

using namespace std;

namespace {

const vector<string> Defaults = {
  "rnsmksnr/8/pppppppp/8/8/PPPPPPPP/8/RNSKMSNR w 0 1",
  "3s1k2/m4n2/1s1n1p2/p2p1Pp1/Pp1P2P1/1P1SMS1r/7N/3K2NR b 0 1",
  "r1sm1r2/3k1s1R/1pp2p2/P1nnPP2/7p/PS3N2/3NSM1P/2RK4 w 0 1",
  "3m4/4s2k/2R1p3/2S2pM1/p2NnP2/4P3/4K3/1r6 b 12 45",
  "3r3r/2snm1k1/5pp1/1Pp5/p1S1PP1p/P1S3PP/K4M2/3R3R b 1 25",
  "6r1/2mnks2/pps1pn1p/2pp1p2/1PNP1P2/P1PKPS1P/2S1N3/R3M3 w 0 16",
  "8/8/5k2/7p/8/6RR/3K4/8 w 0 1 moves h3h5" // draw by counting rules
};

} // namespace

/// setup_bench() builds a list of UCI commands to be run by bench. There
/// are five parameters: TT size in MB, number of search threads that
/// should be used, the limit value spent for each position, a file name
/// where to look for positions in FEN format and the type of the limit:
/// depth, perft, nodes and movetime (in millisecs).
///
/// bench -> search default positions up to depth 13
/// bench 64 1 15 -> search default positions up to depth 15 (TT = 64MB)
/// bench 64 4 5000 current movetime -> search current position with 4 threads for 5 sec
/// bench 64 1 100000 default nodes -> search default positions for 100K nodes each
/// bench 16 1 5 default perft -> run a perft 5 on default positions

vector<string> setup_bench(const Position& current, istream& is) {

  vector<string> fens, list;
  string go, token;

  // Assign default values to missing arguments
  string ttSize    = (is >> token) ? token : "16";
  string threads   = (is >> token) ? token : "1";
  string limit     = (is >> token) ? token : "14";
  string fenFile   = (is >> token) ? token : "default";
  string limitType = (is >> token) ? token : "depth";

  go = "go " + limitType + " " + limit;

  if (fenFile == "default")
      fens = Defaults;

  else if (fenFile == "current")
      fens.push_back(current.fen());

  else
  {
      string fen;
      ifstream file(fenFile);

      if (!file.is_open())
      {
          cerr << "Unable to open file " << fenFile << endl;
          exit(EXIT_FAILURE);
      }

      while (getline(file, fen))
          if (!fen.empty())
              fens.push_back(fen);

      file.close();
  }

  list.emplace_back("ucinewgame");
  list.emplace_back("setoption name Threads value " + threads);
  list.emplace_back("setoption name Hash value " + ttSize);

  for (const string& fen : fens)
      if (fen.find("setoption") != string::npos)
          list.emplace_back(fen);
      else
      {
          list.emplace_back("position fen " + fen);
          list.emplace_back(go);
      }

  return list;
}
