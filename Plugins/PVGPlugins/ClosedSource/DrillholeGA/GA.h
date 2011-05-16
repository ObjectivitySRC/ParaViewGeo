/*=========================================================================
MIRARCO MINING INNOVATION
Author: Chris Cameron (ccameron@mirarco.org)
===========================================================================*/

#ifndef __DrillholeGA_h
#define __DrillholeGA_h

/** \file GA.h
    \brief Header for all Genetic Algorithm-related API.

    \todo REFACTOR: Make a Score class to encapsulate the crazy backwards logic of 
                    score comparisons (e.g. greater than is actually less than...)

    \todo REFACTOR: Make a Component class to encapsulate a Drillhole's parameters,
                    i.e., abstract away the fact that they are just doubles in a vector. 
*/

#include <vector>

/**
  \var NO_SCORE
  \brief This score identifies an <code>Individual</code> that has not yet been evaluated. 
  
  That may mean the <code>Individual</code> is: 

  -# Not completely initialized, or
  -# Completely initialized, but not yet evaluated by the fitness function.
*/
const double NO_SCORE = -99999;

/**
  \class Drillhole
  \brief A <code>Drillhole</code> represents a real drillhole by its components.

  Although the current implementation makes room for an arbitrary number of components, 
  technically there will only ever be six (in the following order):
  -# X position of collar point
  -# Y position of collar point
  -# Z position of collar point
  -# Azimuth angle
  -# Dip angle
  -# Length of the drillhole
*/
class Drillhole
  {
  public:

    /**
      This represents the components of this <code>Drillhole</code>. Currently X, Y, Z, Azimuth, Dip, Length.
    */
    std::vector<double> drillhole;

    /**
      \typedef iterator
      Makes it easier to iterate a <code>Drillhole</code>'s components.
    */
    typedef std::vector<double>::iterator iterator;

    /**
      Default Constructor
    */
    Drillhole () { }

    /**
      Parameterized Constructor.
      \param d The <code>vector</code> of components that will be copied into this <code>Drillhole</code>.
    */
    Drillhole(std::vector<double> d):drillhole(d) {}

    /**
      Copy Constructor. Ensures that modifications to this <code>Drillhole</code> don't affect <code>d</code>.

      \param d The <code>Drillhole</code> that will be duplicated.
    */
    Drillhole (const Drillhole &d)
      {
      int size = d.drillhole.size();
      drillhole.resize(size);
      std::copy(d.drillhole.begin(), d.drillhole.end(), drillhole.begin());
      }

    /**
      Copy assignment operator. Ensures that modifications to this <code>Drillhole</code> don't affect <code>d</code>.

      \param d The <code>Drillhole</code> that will be duplicated.
    */
    Drillhole& operator = (const Drillhole &d)
      {
      if (drillhole.size() > 0)
        drillhole.clear();
      int size = d.drillhole.size();
      drillhole.resize(size);
      std::copy(d.drillhole.begin(), d.drillhole.end(), drillhole.begin());
      return *this;
      }

    /**
      Copy access operator.

      Assignment not possible with this version. Modifying the returned component does not 
      affect the <code>Drillhole</code>'s copy.
      \param index The index into the internal <code>vector</code> of components.
    */
    double operator[](int index) const;

    /**
      Reference access operator.
      This version allows assignment and modification.
      \param index The index into the internal <code>vector</code> of components.
    */
    double& operator[](int index);

    /**
      Copies the <code>vector</code> of components from <code>drillhole</code> into this <code>Drillhole</code>

      \param drillhole The <code>vector</code> of components to copy from.

      \todo REFACTOR: Isn't there a way to make another "Copy Assignment Operator (=)" that can take a vector of doubles? This method seems like an ugly way to handle something that operators can do prettier.

    */
    void SetDrillhole(std::vector<double> drillhole)
      {
      drillhole.clear();
      int size = drillhole.size();
      drillhole.resize(size);
      std::copy(drillhole.begin(), drillhole.end(), this->drillhole.begin());
      }

    /**
      Returns a <code>Drillhole::iterator</code> pointing to the first component in the internal <code>vector</code> of components.

      \see Drillhole::iterator
    */
    std::vector<double>::iterator begin()
      {
      return drillhole.begin();
      }

    /**
      Returns a <code>Drillhole::iterator</code> pointing to the position after the last component in the internal <code>vector</code> of components.

      \see Individual::iterator
    */
    std::vector<double>::iterator end()
      {
      return drillhole.end();
      }

    /**
      Returns the number of components that are in this <code>Drillhole</code>.
    */
    int size()
      {
      return drillhole.size();
      }

  };

/**
  \class Individual
  \brief An <code>Individual</code> represents a group of <code>Drillhole</code>s.

  The <code>Drillhole</code>s may not be completely initialized, though this should be avoided. 

  The <code>score</code> may be
  -# <code>GA::NO_SCORE</code>
  -# initialized
*/
class Individual
  {
  public:
    /**
      This represents the group of <code>Drillhole</code>s that make up this <code>Individual</code>.
    */
    std::vector<Drillhole> drillholes;

    /**
      \typedef iterator
      Makes it easier to iterate <code>Drillhole</code>s that make up this <code>Individual</code>
    */
    typedef std::vector<Drillhole>::iterator iterator;

    /**
      \var score
      This <code>score</code> represents the fitness value assigned to this <code>Individual</code>.
      \see GA::NO_SCORE
    */
    double score;

    /**
      Default Constructor.

      Note that <code>score</code> will be initialized to <code>NO_SCORE</code>.
      \see GA::NO_SCORE
    */
    Individual() : score(NO_SCORE) {}

    /**
      Parameterized Constructor.
      Note that <code>score</code> will be initialized to <code>NO_SCORE</code>.
      \see GA::NO_SCORE
      \param d The <code>vector</code> of <code>Drillhole</code>s that will be copied into this <code>Individual</code>.
    */
    Individual(std::vector<Drillhole> d): score(NO_SCORE), drillholes(d) {}

    /**
      Parameterized Constructor.
      \param d The <code>vector</code> of <code>Drillhole</code>s that will be copied into this <code>Individual</code>.
      \param s The <code>score</code> to assign to this <code>Individual</code>.
    */
    Individual(std::vector<Drillhole> d, double s): score(s), drillholes(d) {}

     /**
      Copy Constructor. Ensures that modifications to this <code>Individual</code> don't affect <code>i</code>.

      \param i The <code>Individual</code> that will be duplicated.
    */
    Individual (const Individual &i)
      {
      int size = i.drillholes.size();
      drillholes.resize(size);
      std::copy(i.drillholes.begin(), i.drillholes.end(), drillholes.begin());
      score = i.score;
      }

    /**
      Copy assignment operator. Ensures that modifications to this <code>Individual</code> don't affect <code>i</code>.

      \param i The <code>Individual</code> that will be duplicated.
    */
    Individual& operator = (const Individual &i)
      {
      if (drillholes.size() > 0)
        drillholes.clear();
      int size = i.drillholes.size();
      drillholes.resize(size);
      std::copy(i.drillholes.begin(), i.drillholes.end(), drillholes.begin());
      score = i.score;
      return *this;
      }

    /**
      Copy access operator.

      Assignment not possible with this version. Modifying the returned <code>Drillhole</code> does not 
      affect the <code>Individual</code>'s copy.
      \param index The index into the internal <code>vector</code> of <code>Drillhole</code>s.
    */
    Drillhole operator[](int index) const;
    
    /**
      Reference access operator.
      This version allows assignment and modification.
      \param index The index into the internal <code>vector</code> of <code>Drillhole</code>s.
    */
    Drillhole& operator[](int index);

    /**
      Returns an <code>Individual::iterator</code> pointing to the first <code>Drillhole</code> in the internal <code>vector</code> of <code>Drillhole</code>s.

      \see Individual::iterator
    */
    std::vector<Drillhole>::iterator begin()
      {
      return drillholes.begin();
      }

    /**
      Returns an <code>Individual::iterator</code> pointing to the position after the last <code>Drillhole</code> in the internal <code>vector</code> of <code>Drillhole</code>s.

      \see Individual::iterator
    */
    std::vector<Drillhole>::iterator end()
      {
      return drillholes.end();
      }

    /**
      Returns the number of <code>Drillhole</code>s that are in this <code>Individual</code>.
    */
    int size()
      {
      return drillholes.size();
      }

  };



/**
  \class Population
  \brief A <code>Population</code> represents a group of <code>Individual</code>s.
         
  The <code>Individual</code>s may not be completely initialized, and may not have been evaluated by a fitness function yet.
*/
class Population
  {
  public:

    /**
      This represents the group of <code>Individual</code>s that make up this <code>Population</code>.
    */
    std::vector<Individual> individuals;

    /** 
        \typedef iterator 
        Makes it easier to iterate <code>Individual</code>s in this <code>Population</code>. 
        Thus a <code>Population::iterator</code> iterates pointers to <code>Individual</code>s.
    */
    typedef std::vector<Individual>::iterator iterator; 

    /**
      Default Constructor
    */
    Population () { }

    /**
      Parameterized Constructor.

      \param i The list of <code>Individual</code>s that will be copied into this <code>Population</code>.
    */
    Population(std::vector<Individual> i):individuals(i) {}

    /**
      Copy Constructor. Ensures that modifications to this <code>Population</code> don't affect <code>p</code>.

      \param p The <code>Population</code> that will be duplicated.
    */
    Population (const Population &p)
      {
      int size = p.individuals.size();
      individuals.resize(size);
      std::copy(p.individuals.begin(), p.individuals.end(), individuals.begin());
      }

    /**
      Copy assignment operator. Ensures that modifications to this <code>Population</code> don't affect <code>p</code>.

      \param p The <code>Population</code> that will be duplicated.
    */
    Population operator = (const Population &p)
      {
      individuals.push_back(Individual());
      if (individuals.size() > 0)
        individuals.clear();
      int size = p.individuals.size();
      individuals.resize(size);
      std::copy(p.individuals.begin(), p.individuals.end(), individuals.begin());
      return *this;
      }

    /**
      Copy insertion operator. The <code>Individual</code>s from <code>p</code> are appended to the end of the list of 
      <code>Individual</code>s in this <code>Population</code>. Modifications to this <code>Population</code> and its
      updated list of <code>Individual</code>s should have no effect on <code>p</code> or its <code>Individual</code>s.

      \param p The <code>Population</code> that <code>Individual</code>s should be copied from.
    */
    Population operator += (const Population &p)
      {
      Population copy = p; //get a copy first
      std::vector<Individual>::iterator end = individuals.end();
      //individuals.resize(individuals.size() + copy.individuals.size());
      individuals.insert(end, copy.individuals.begin(), copy.individuals.end());
      return *this;
      }

    /** 
      "Better" operator. This method should return whether this <code>Population</code>'s best score is better than <code>p</code>'s best score.

      Note that we are currently minimizing scores, so the code logic may appear backwards to you at first.

      \param p The <code>Population</code> to compare scores with.

      \return true if this <code>Population</code>'s best score is better than <code>p</code>'s best score.
      \return false otherwise.
    */
    bool operator>(Population &p)
      {
      return best() < p.best();
      }

    /**
      Copy access operator.

      Assignment not possible with this version. Modifying the returned <code>Individual</code> does not 
      affect the <code>Population</code>'s copy.
      \param index The index into the internal <code>vector</code> of <code>Individual</code>s.
    */
    Individual operator[](int index) const;
    /**
      Reference access operator.
      This version allows assignment and modification.
      \param index The index into the internal <code>vector</code> of <code>Individual</code>s.
    */
    Individual& operator[](int index);

    /** 
      Replace this <code>Population</code>'s worst <code>Individual</code>s with the better <code>Individual</code>s 
      from <code>p</code>.

      \param p The <code>Population</code> with the (potentially) better <code>Individual</code>s.

    */
    void Update(Population &p);

    /**
      Erases the worst <code>Individual</code> from this <code>Population</code>'s <code>vector</code> of 
      <code>Individual</code>s. 
      
      The method's naming is misleading, since Pop usually means the item being removed would be returned.
    */
    void EraseWorst();

    /**
      Returns the "best" score in this <code>Population</code>. In the current implementation, returns the
      lowest score, because we are minimizing that score.

      \return The best score in this <code>Population</code>.
    */
    double best();

    /**
      Returns a <code>Population::iterator</code> pointing to the <code>Individual</code> with the "best" score in this <code>Population</code>. In the current implementation, that <code>Individual</code> will have the lowest score, because we are minimizing that score.

      \return A <code>Population::iterator</code> pointing to the <code>Individual</code> with the best score in this <code>Population</code>.
    */
    iterator best_element();

    /**
      Returns the "worst" score in this <code>Population</code>. In the current implementation, returns the
      highest score, because we are minimizing that score.

      \return The worst score in this <code>Population</code>.
    */
    double worst();

    /**
      Returns the sum of all scores in this <code>Population</code>.
      Note that this would include any scores that may not have been initialized.
      \see GA::NO_SCORE

      \return The sum of all scores in this <code>Population</code>.
    */
    double sum();

    /**
      Returns a <code>Population::iterator</code> pointing to the first <code>Individual</code> in the internal <code>vector</code> of <code>Individual</code>s.

      \see Population::iterator
    */
    std::vector<Individual>::iterator begin()
      {
      return individuals.begin();
      }

    /**
      Returns a <code>Population::iterator</code> pointing to the position after the last <code>Individual</code> in the internal <code>vector</code> of <code>Individual</code>s.

      \see Population::iterator
    */
    std::vector<Individual>::iterator end()
      {
      return individuals.end();
      }

    /**
      Returns the number of <code>Individual</code>s that are in this <code>Population</code>.
    */
    int size()
      {
      return individuals.size();
      }

  };

#endif