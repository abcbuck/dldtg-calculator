#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>
#include <iterator>
#include <list>
#include <set>
#include <map>
#include <vector>

std::vector<bool> stringToVectorBool(std::string&);
std::vector<bool> nand(std::vector<bool> const&, std::vector<bool> const&);
template <class T> bool isSubset(std::set<T> const&, std::set<T> const&);
void print(std::set<std::vector<bool>>&);
void print(std::list<std::vector<bool>>&);
std::string toString(std::vector<bool>);
void printReadable(std::set<std::vector<bool>>, std::set<std::vector<bool>> const& targets);

int main() {
  /*define desired solution by truth table, define truth table by series of binary digits, e.g.:
  01010011 =
  A B C  Z
  1 1 1  1
  1 1 0  1
  1 0 1  0
  1 0 0  0
  0 1 1  1
  0 1 0  0
  0 0 1  1
  0 0 0  0
  */
  std::set<std::vector<bool>> desiredTruthTables;
  {
    int numberOfTables;

    std::cout << "Please enter the number of desired truth tables: ";
    std::cin >> numberOfTables;

    for (int i = 1; i <= numberOfTables; i++) {
      std::string sDesiredTruthTable;
      std::cout << "Please enter desired truth table " << i << ": ";
      std::cin >> sDesiredTruthTable;
      desiredTruthTables.insert(stringToVectorBool(sDesiredTruthTable));
    }
  }
  //test for consistency
  for (auto i = desiredTruthTables.begin(); std::next(i) != desiredTruthTables.end(); i++)
    if (i->size() != std::next(i)->size()) {
      std::cerr << "Error: Inconsistent truth tables. Please specify same-sized truth tables.";
      return EXIT_FAILURE;
    }

  const int numberOfInputs = log2(desiredTruthTables.begin()->size());
  std::cout << "Number of inputs: " << numberOfInputs << std::endl << std::endl
    << "Initializing...";
  //every circuit is a set of truth tables
  std::set<std::set<std::vector<bool>>> circuits;
  std::set<std::vector<bool>> startCircuit;
  {
    //construct start circuit
    std::vector<bool>* inputs = new std::vector<bool>[numberOfInputs];
    for (int i = 0; i < desiredTruthTables.begin()->size(); i++) {
      for (int j = 0; j < numberOfInputs; j++)
        inputs[j].emplace_back(i >> j & 1); //jth bit from the right
    }
    //truth tables for the inputs
    for (int i = 0; i < numberOfInputs; i++)
      startCircuit.emplace(inputs[i]);
    delete[] inputs;
  }
  circuits.emplace(startCircuit);
  //add all possible circuits for the amount nandAmount=1 of nands,
  //continually increase this number until a circuit with the desired truth table has been found
  bool foundIt = false;
  for (int nandAmount = 1; ; nandAmount++) {
    std::cout << " done.  Number of circuits in memory: " << circuits.size() << std::endl
              << "Creating circuits with a total of " << nandAmount << " NAND gates...";
    std::set<std::set<std::vector<bool>>> newCircuits;
    for (auto i = circuits.begin(); i != circuits.end(); i++) {
      for (auto j = i->begin(); j != i->end(); j++) {
        for (auto k = j; k != i->end(); k++) {
          std::set<std::vector<bool>> newCircuit = *i;
          std::vector<bool> sNand = nand(*j, *k);
          if (std::find(newCircuit.begin(), newCircuit.end(), sNand) != newCircuit.end() || std::find(startCircuit.begin(), startCircuit.end(), sNand) != startCircuit.end())
            continue; //new gate is already part of circuit
          newCircuit.emplace(sNand); //add new gate
          if (isSubset(desiredTruthTables, newCircuit)) {
            std::cout << std::endl
                      << std::endl
                      << "Found solution of size " << nandAmount << "!!" << std::endl;
            print(newCircuit);
            printReadable(newCircuit, desiredTruthTables);
            //truth table has been found
            return EXIT_SUCCESS;
          }
          newCircuits.emplace(newCircuit);
        }
      }
    }
    circuits = std::move(newCircuits);
  }
}

std::vector<bool> stringToVectorBool(std::string& s) {
  std::vector<bool> ret;
  for (int i = 0; i < s.length(); i++)
    ret.push_back(s[i] == '1');
  ret.shrink_to_fit();
  return ret;
}

std::vector<bool> nand(std::vector<bool> const& a, std::vector<bool> const& b) {
  std::vector<bool> ret;
  ret.reserve(a.size());
  for (int i = 0; i < a.size(); i++)
    ret.push_back(!(a[i] && b[i]));
  return ret;
}

template <class T>
bool isSubset(std::set<T> const& a, std::set<T> const& b) {
  for (auto i = a.begin(); i != a.end(); i++)
    if (std::find(b.begin(), b.end(), *i) == b.end())
      return false;
  return true;
}

void print(std::set<std::vector<bool>>& a) {
  for (auto i = a.cbegin(); i != a.cend(); i++)
    std::cout << toString(*i) << std::endl;
  std::cout << std::endl;
}
void print(std::list<std::vector<bool>>& a) {
  for (auto i = a.cbegin(); i != a.cend(); i++)
    std::cout << toString(*i) << std::endl;
  std::cout << std::endl;
}
std::string toString(std::vector<bool> a) {
  std::string ret;
  for (auto i = a.cbegin(); i != a.cend(); i++)
    ret += '0'+*i;
  return ret;
}

void printReadable(std::set<std::vector<bool>> b, std::set<std::vector<bool>> const& targets) {
  std::cout << "Netlist definition:" << std::endl
            << "DEF FUNCTION_NAME" << std::endl;

  const int numberOfInputs = std::log2(b.begin()->size());
  
  std::list<std::vector<bool>> a;
  //store the circuit inputs in a, delete them from b
  for(int i=1; i<=numberOfInputs; i++) {
    std::vector<bool> vb;
    for(int j=0; j<b.begin()->size(); j++)
      vb.emplace_back(j % static_cast<int>(std::pow(2, i))  >=  std::pow(2, i-1));
    a.emplace_front(vb);
    b.erase(vb);
  }

  std::map<std::vector<bool>, std::string> nets;
  {
    //print input ports
    auto toMakeReadable = a.begin();
    for(int i=0; i<numberOfInputs; i++, toMakeReadable++) {
      std::string portName(1, 'A'+i);
      nets.insert({*toMakeReadable, portName});
      std::cout << "  PORT IN " << portName << " # " << toString(*toMakeReadable) << std::endl;
    }
  }
  {
    //print output ports
    auto toMakeReadable = b.begin();
    for(int i=targets.size()-1; i>=0; i--, toMakeReadable++) {
      while(!targets.contains(*toMakeReadable))
        toMakeReadable++;
      std::string portName(1, 'Z'-i);
      nets.insert({*toMakeReadable, portName});
      std::cout << "  PORT OUT " + portName + " # " + toString(*toMakeReadable) << std::endl;
    }
  }
  std::cout << std::endl;
  {
    //print nets
    auto toMakeReadable = b.begin();
    for(int i=1; i <= b.size()-targets.size(); i++, toMakeReadable++) {
      while(targets.contains(*toMakeReadable))
        toMakeReadable++;
      std::cout << "  NET N" << i << std::endl;
    }
  }
  std::cout << std::endl;
  //from this point onwards *a* stores the gates that have been printed and *b* stores the remaining gates
  {
    int i=0, n=0;
    next:
    for(auto toMakeReadable = b.begin(); toMakeReadable!=b.end(); toMakeReadable++)
      for(auto jt=a.begin(); jt!=a.end(); jt++)
        for(auto kt=jt; kt!=a.end(); kt++)
          if(nand(*jt, *kt) == *toMakeReadable) {
            std::string netName;
            if(!targets.contains(*toMakeReadable)) {
              n++;
              netName = "N" + std::to_string(n);
              nets.insert({*toMakeReadable, netName});
            }
            else
              netName = nets[*toMakeReadable]; //portName
            i++;
            std::cout << "  INST I" + std::to_string(i) + " NAND " + nets[*jt] + " " + nets[*kt] + " " + netName + " # " + toString(*toMakeReadable) << std::endl;
            a.push_back(*toMakeReadable);
            b.erase(*toMakeReadable);
            goto next;
          }
  }
  std::cout << "ENDDEF" << std::endl;
}
