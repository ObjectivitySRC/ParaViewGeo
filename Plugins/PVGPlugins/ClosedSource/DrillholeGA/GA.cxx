#include "GA.h"

//----------------------------------------------------------------------------
// best - Returns the 'best' score in the Population. best != highest
//----------------------------------------------------------------------------
double Population::best()
  {
  double best = individuals[0].score;
  for (iterator i = this->begin() + 1; i < this->end(); ++i)
    {
    best = std::min(best, i->score);
    }
  return best;
  }

//----------------------------------------------------------------------------
// best_index - Returns the 'best' score's index in the Population
//----------------------------------------------------------------------------
Population::iterator Population::best_element()
  {
  iterator begin = this->begin();
  iterator best = this->begin();
  for (iterator i = best + 1; i < this->end(); ++i)
  {
    if (i->score < best->score)
      best = i;
    }
  return best;
  }

//----------------------------------------------------------------------------
// worst - Returns the 'worst' score in the Population. worst != lowest
//----------------------------------------------------------------------------
double Population::worst()
  {
  double worst = individuals[0].score;
  for (iterator i = this->begin() + 1; i < this->end(); ++i)
    {
    worst = std::max(worst, i->score);
    }
  return worst;
  }

//----------------------------------------------------------------------------
// sum - Returns the sum of all scores
//----------------------------------------------------------------------------
double Population::sum()
  {
  double sum = 0;
  for (iterator i = this->begin() + 1; i < this->end(); ++i)
    {
    sum += i->score;
    }
  return sum;
  }

//----------------------------------------------------------------------------
// access operator and access/assignment operator
//----------------------------------------------------------------------------
Individual Population::operator[](int index) const
  {
  return this->individuals[index];
  }
Individual& Population::operator[](int index)
  {
  return this->individuals[index];
  }

//----------------------------------------------------------------------------
// access operator and access/assignment operator
//----------------------------------------------------------------------------
Drillhole Individual::operator[](int index) const
  {
  return this->drillholes[index];
  }

Drillhole& Individual::operator[](int index)
  {
  return this->drillholes[index];
  }

//----------------------------------------------------------------------------
// access operator and access/assignment operator
//----------------------------------------------------------------------------
double Drillhole::operator[](int index) const
  {
  return this->drillhole[index];
  }

double& Drillhole::operator[](int index)
  {
  return this->drillhole[index];
  }

//----------------------------------------------------------------------------
// Update - add only better individuals to self
//----------------------------------------------------------------------------
void Population::Update(Population &p)
  {
  if (p > (*this)) //only if there is something better to add
    {
    double worst = this->worst();
    for (Population::iterator it = p.begin(); it < p.end(); ++it)
      {
      if ((*it).score < worst)
        {
        //erase worst
        this->EraseWorst();
        //add (*it) to vector
        this->individuals.push_back(Individual(*it));
        //update worst (we do it like this, because worst() is O(n) algorithm
        worst = this->worst();
        }
      }
    }
  }

//----------------------------------------------------------------------------
// EraseWorst - get rid of the worst individual
//----------------------------------------------------------------------------
void Population::EraseWorst()
  {
  double worst = this->worst();
  for (iterator it = this->begin(); it < this->end(); ++it)
    {
    if ((*it).score == worst)
      {
      this->individuals.erase(it);
      return;
      }
    }
  }