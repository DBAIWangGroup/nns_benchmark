// Copyright (C) 2010 Michael E. Houle
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

/*!
 * @mainpage Rank Cover Tree Library
 *
 * @section intro Introduction
 *
 * This package provides a C++ implementation of the rank cover tree (see [1]),
 * a probabilistic algorithm for finding k-nearest neighbors of point sets in 
 * general metric spaces. The rank cover tree reinterprets the cover set 
 * analysis of the cover tree search structure in terms of neighbor ranks as 
 * measured from the query point, rather than explicit distances. The direct 
 * analysis results in a construction and query time complexity with a far 
 * smaller dependence on the measure of implicit dimension used in the cover 
 * tree analysis. The rank cover tree can also be viewed as a variant of the 
 * generic, scalable SASH heuristic for similarity search, where the rank 
 * cover tree parameter governing the execution have been set to a value 
 * insufficiently large to guarantee correctness with high probability. In this 
 * sense, this implementation and its paper constitute the first formal 
 * analysis of the accuracy of the SASH.
 *
 * This source package provides the following files:
 * -# DistData.h
 * -# rct.h
 * -# rct.cpp
 * -# random.h
 * -# random.cpp
 *
 * <hr />
 *
 * @section notes Release Notes
 *
 * If you like to use or extend this package, please be aware of any material
 * available on <a href="http://tcs.rwth-aachen.de/~nett/">this website</a>.
 *
 * <hr />
 *
 * @section disclaimer Disclaimer
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. The source code and derived binary forms may be used only for
 *    non-commercial, non-profit research purposes.
 *
 * 2. Redistributions of source code must retain the above copyright
 *    notice, these conditions, and the following disclaimer.
 *
 * 3. Redistributions in binary form must reproduce the above copyright
 *    notice, these conditions, and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 4. The names of its contributors may not be used to endorse or promote
 *    products derived from this software without specific prior written
 *    permission.
 *
 * <em>This software is provided by the copyright holders and contributors
 * "as is" and any express or implied warranties, including, but not limited
 * to, the implied warranties of merchantability and fitness for a particular
 * purpose are disclaimed. In no event shall the copyright owner or 
 * contributors be liable for any direct, indirect, incidental, special,
 * exemplary, or consequential damages (including, but not limited to, 
 * procurement of substitute goods or services; loss of use, data, or
 * profits; or business interruption) however caused and on any theory of
 * liability, whether in contract, strict liability, or tort (including 
 * negligence or otherwise) arising in any way out of the use of this software,
 * even if advised of the possibility of such damage.</em>
 *
 * Comments, bug fixes, etc welcome!
 * Contact e-mail addresses: meh@nii.ac.jp, meh@acm.org, michael.nett@rwth-aachen.de
 *
 * <hr />
 *
 * @section usage Example of usage
 *
 * <em>The following examples assume that the array <code>DistData** data</code> contains
 * (at least) <code>size</code> data items.</em>
 *
 * In order to create an empty RCT use the following code.
 *
 * \code
 * RCT* rct = new RCT();
 * \endcode
 *
 * Alternatively, a specific seed value for the random number generator can be
 * passed as an argument to the constructor:
 *
 * \code
 * RCT* rct = new RCT(time(0));
 * \endcode
 *
 * After creation of an RCT, the verbosity level can be set (here we want all error
 * and progress messages):
 *
 * \code
 * rct->setVerbosity(2);
 * \endcode
 *
 * You might want to use a coverage parameter other than the default (<em>1.0</em>),
 * depending on the intrinsic dimension of your dataset. The coverage parameter is
 * changed using the following method:
 *
 * \code
 * rct->setCoverageParameter(8.0);
 * \endcode
 *
 * In addition, the sample rate (which has the default value of <em>2.0</em>) can be
 * changed. To generate an RCT with an expected number of levels of <em>3</em>, we
 * use the following setting:
 *
 * \code
 * rct->setSampleRate(pow(size, 1.0 / 3.0));
 * \endcode
 * 
 * Now the RCT can be constructed by calling 
 *
 * \code
 * rct->build(data, size);
 * \endcode
 *
 * After the construction finishes we can find approximate <em>k</em>
 * nearest-neighbors to a query object <code>DistData* query</code> by
 * calling:
 *
 * \code
 * int num = rct->findNear(query, k);
 * \endcode
 *
 * In the following we can access the indices and query-to-item distances 
 * of the query results.
 *
 * \code
 * int* indices = new int [num];
 * rct->getResultIndices(indices, num);
 * float* distances = new float [num];
 * rct->getResultDists(distances, num);
 * \endcode
 */

#ifndef __RCT_H
#define __RCT_H

#include <iostream>
#include <sstream>
#include <fstream>

#include <cassert>

#include "DistData.h"

using namespace std;

#ifndef RCT_NONE_
#define RCT_NONE_ (-1)
#endif

#ifndef RCT_UNDEFINED_
#define RCT_UNDEFINED_ (-1)
#endif

#ifndef RCT_UNKNOWN_
#define RCT_UNKNOWN_ (-1.0F)
#endif

#ifndef RCT_BUFSIZE_
#define RCT_BUFSIZE_ (1024)
#endif

#ifndef RCT_VERSION_
#define RCT_VERSION_ ("1.0")
#endif

#include "random.h"

/*!
 * @class   RCT
 * @author  Michael E. Houle, Michael Nett
 * @date    05/25/2010
 * @version 1.0
 *
 * Implementation of probabilistically-correct index for
 * similarity search.
 *
 * <em>&copy; 2006-2010 Michael E. Houle &mdash; All rights reserved.</em>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * -# The source code and derived binary forms may be used only for
 *    non-commercial, non-profit research purposes.
 * -# Redistributions of source code must retain the above copyright
 *    notice, these conditions, and the following disclaimer.
 * -# Redistributions in binary form must reproduce the above copyright
 *    notice, these conditions, and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * -# The names of its contributors may not be used to endorse or promote
 *    products derived from this software without specific prior written
 *    permission.
 *
 * <em>This software is provided by the copyright holders and contributors
 * "as is" and any express or implied warranties, including, but not limited
 * to, the implied warranties of merchantability and fitness for a particular
 * purpose are disclaimed. In no event shall the copyright owner or 
 * contributors be liable for any direct, indirect, incidental, special,
 * exemplary, or consequential damages (including, but not limited to, 
 * procurement of substitute goods or services; loss of use, data, or
 * profits; or business interruption) however caused and on any theory of
 * liability, whether in contract, strict liability, or tort (including 
 * negligence or otherwise) arising in any way out of the use of this software,
 * even if advised of the possibility of such damage.</em>
 *
 * <b>Comments, bug fixes, etc welcome!</b>
 *
 * Contact e-mail address: meh@nii.ac.jp, meh@acm.org, michael.nett@rwth-aachen.de
 */
//! Rank Cover Tree for probabilistically-correct similarity search.

class RCT {

private:
    //! The truth value 'true'.
    static const int TRUE = 1;

    //! The truth value 'false'.
    static const int FALSE = 0;

    //! Data items.
    /*!
     * The array of data items. Note that the data items from the input
     * array are <b>not</b> copied, so the user has to take care of the
     * memory allocated for the input data and the memory must not be
     * released while the RCT is in use.
     */
    DistData** data;

    //! Pseudo random number generator.
    /*!
     * The pseudo random number generator is seeded with the current the
     * timestamp obtained from <code>::time(0)</code> unless the user
     * supplies a specific seed when calling <code>build(...)</code>.
     */
    Random rand;

    //! Sample rate.
    /*!
     * The probability of an item in level set <em>i</em> also occurring
     * in level set <em>i + 1</em> is the inverse of the sample rate.
     * Therefore, a sample rate of <em>2.0</em> is expected to yield
     * a random leveling where each successive random level contains
     * roughly half as many elements as the level below it. A using the
     * <em>j</em>-th root of the number of items provides a random
     * leveling with expected height <em>j</em>.
     */
    float sampleRate;

    //! Size of the data array.
    /*!
     * The number of data items actually stored in the RCT.
     */
    int size;

    //! Size of the level sets.
    /*!
     * Stores the number of element in the level sets L_0,L_1,... depending
     * on how the items are distributed across levels by the random leveling
     * process.
     */
    int* levelSetSizeList;

    //! Maximum number of parents per node.
    /*!
     * This value fixes the maximum number of parents per node. To obtain
     * a rank cover tree this value must be set to <em>1</em>. Using larger
     * values than <em>1</em> results in a non-tree structure. Larger values
     * can improve search accuracies at the cost of query and construction
     * time complexity.
     */
    int maxParents;

    //! Maximum node degree.
    /*!
     * The maximum number of children per node. These include the virtual
     * supernode constructed on top of the highest level in the tree (in
     * case that level contains more than one item).
     */
    int maxDegree;

    //! Average node degree.
    /*!
     * The average number of children per node. These are determined as a
     * result of the build step.
     */
    float avgDegree;

    //! Mapping between internal and external indices.
    /*!
     * Stores the mapping from internal item indices to external (input)
     * indices. This mapping represents the internal permutation of
     * data items done by the RCT during the random leveling. The original
     * data array is not changed.
     */
    int* internToExternMapping;

    //! Height of the RCT.
    /*!
     * Number of sample levels in the rank cover tree (other than the
     * artificial root's). The bottom rank cover tree level has index
     * <em>0</em>, the artificial root has level <code>levels</code>.
     */
    int levels;

    //! Total number of nodes.
    /*!
     * The total number of nodes that is present in the rank cover tree.
     * This value is expected to be in <code>O(n)</code> whenever the sample
     * rate used in the construction is at least <em>2.0</em>.
     *
     * @note While in design the RCT uses copies of nodes at different levels,
     *       these copies are not made in the actual implementation! Therefore,
     *       the number of nodes becomes a symbolic property of the RCT.
     */
    int numNodes;

    //! Lists of tentative parents per node.
    /*!
     * The parent lists store the tentative parents which are sought during
     * the RCT construction phase. The number of parent sought depend on the
     * setting of <code>maxParents</code>.
     *
     * @note This storage is deallocated after the construction is finished.
     */
    int*** parentIndexLLList;

    //! Tentative parent list lengths.
    /*!
     * These arrays store the number of tentative parents found for each
     * node during the construction phase.
     *
     * @note This storage is deallocated after the construction is finished.
     */
    int** parentLSizeLList;

    //! List of children per node.
    /*!
     * The children lists store the chilren that any node has at any given
     * level. The lists represent an 'inversion' of the parent lists used
     * during construction and are used to decend the RCT during search
     * operations.
     */
    int*** childIndexLLList;

    //! Children list lengths.
    /*!
     * These arrays store the number of children for each node at each level.
     */
    int** childLSizeLList;

    //! The current query object.
    /*!
     * The data item that is subject of the currently processed (most recently
     * processed) query. This marker is also used to identify repeated queries
     * where computation time can be saved.
     */
    DistData* query;

    //! Distance cache.
    /*!
     * Stores distances from the current query item to some data items in the
     * RCT that have been performed during the most recent query.
     */
    float* distFromQueryList;

    //! Item cache.
    /*!
     * Stores indices of items whose distance to the current query object has
     * been determined during the current query.
     */
    int* storedDistIndexList;

    //! Cache length.
    /*!
     * Stores the number of items in the current query cache.
     */
    int numStoredDists;

    //! Number of distance comparisons performed.
    /*!
     * Stores the number of distance comparisons that have been
     * performed during the most recent RCT operation.
     */
    unsigned long numDistComps;

    //! Level quota list.
    /*!
     * Stores the maximum number of nodes that are retained in the
     * cover set during a search. The quotas are calculated for each
     * search based on the number of neighbours sought.
     *
     * @todo If we safe the value of 'howMany' used in the queries,
     *       we don't have to recompute that every time (minor
     *       savings).
     */
    int* levelQuotaList;

    //! Coverage parameter.
    /*!
     * The coverage paremeter determines the amount of items that
     * are sought during search and construction operations. Although
     * a higher value is can increase search accuracy and is more likely
     * to produce a well-formed RCT, it comes at the cost of increased
     * search and construction time complexity.
     */
    float coverageParameter;

    //! Query result items.
    /*!
     * Stores the indices of the items found during the most
     * recent search operation.
     */
    int* queryResultIndexList;

    //! Query result distances.
    /*!
     * Stores the query-to-result distances of the items found during
     * the most recent search operation.
     */
    float* queryResultDistList;

    //! Query result size.
    /*!
     * Stores the number of results found in the most recent search
     * operation.
     */
    int queryResultSize;

    //! Query result sample size.
    /*!
     * The number of sample items within which the most recent similarity
     * search was performed.
     */
    int queryResultSampleSize;

    //! Temporary index list.
    /*!
     * This list stores nodes visited during the search operation.
     */
    int* visitedNodeIndexList;

    //! Temporary distance list.
    /*!
     * This list stores the distance to the query item of nodes already
     * visited during the current search.
     */
    float* tempResultDistList;

    //! Temporary result list.
    /*!
     * This list stores result candidates during the search operation.
     */
    int* tempResultIndexList;

    //! Verbosity levle.
    /*!
     * The verbosity level of the RCT determines the amount of feedback
     * provided to the user. Values equal to or less than <em>0</em> result
     * in no feedback at all. A value of <em>1</em> makes the RCT report
     * only errors, whereas a value of <em>2</em> generates messages for
     * errors as well as progress. Finally, a value of <em>3</em> reports
     * errors, progress and debug output.
     */
    int verbosity;

    //! Build scale factor.
    /*!
     * The build scale factor determine the scaling that was used during
     * the construction of the RCT.
     */
    float buildScaleFactor;

    //! Random number generator seed.
    /*!
     * The seed value used to initialize the random number generator.
     */
    unsigned long seed;

public:
	int checks;
    //! Retrieve the fraction of edges in the RCT which are well-formed.
    double getFractionOfWellformedEdges();

    //! Create an RCT with a specific seed.
    RCT(const unsigned long& seed = 3141569UL);

    //! Destroy the RCT.
    virtual ~RCT();

    //! Set the sample rate.
    void setSampleRate(const float& sampleRate);

    //! Constructs an RCT from an array of data items.
    int build(DistData** inputData, const int& numItems, const float& scaleFactor = 1.0f, const int& numParents = 1);

    //! Loads a previously-saved RCT from a file.
    int build(const char* fileName, DistData** inputData, const int& numItems);

    //! Perform an exact range query.
    int findAllInRange(DistData* query, const float& limit, const int& sampleLevel = 0);

    //! Perform an approximate range query.
    int findMostInRange(DistData* query, const float& limit, const float& scaleFactor = 1.0f, const int& sampleLevel = 0);

    //! Perform an approximate nearest-neighbor query.
    int findNear(DistData* query, const int& howMany = 1, const float& scaleFactor = 1.0f, const int& sampleLevel = 0);

    //! Perform an exact nearest-neighbor query.
    int findNearest(DistData* query, const int& howMany = 1, const int& sampleLevel = 0);

    //! Retrieve the average node degree.
    float getAvgDegree() const;
    
    //! Retrieve the build scale factor.
    float getBuildScaleFactor() const;

    //! Retrieve the coverage parameter.
    float getCoverageParameter() const;

    //! Retrieve the data items.
    DistData** getData();

    //! Retrieve a mapping from external to internal item indices.
    int getExternToInternMapping(int* result, int capacity) const;

    //! Retrieve a mapping from internal to external item indices.
    int getInternToExternMapping(int* result, int capacity) const;

    //! Retrieve the level set sizes.
    int getLevelSetSizes(int* result, int capacity) const;

    //! Retrieve the maximum node degree.
    int getMaxDegree() const;

    //! Retrieve the height of the items in the RCT.
    int getMaxLevelAssignment(int* result, int capacity) const;

    //! Retrieve the maximum number of parents allowed.
    int getMaxParents() const;
    
    //! Retrieve the number of items in the RCT.
    int getNumItems() const;

    //! Retrieve the number of levels in the RCT.
    int getNumLevels() const;

    //! Retrieve the number of nodes in the RCT.
    int getNumNodes() const;

    //! Computes the recall accuracy.
    float getResultAcc(float* exactDistList, int howMany) const;

    //! Retrieve the query-to-neighbor distances.
    int getResultDists(float* result, int capacity) const;

    //! Retrieve the number of distance comparisons performed.
    unsigned long getResultDistComps() const;

    //! Retrieve the indices of the query result items.
    int getResultIndices(int* result, int capacity) const;

    //! Retrieve the number of results found.
    int getResultNumFound() const;

    //! Retrieve the sample size used in the query.
    int getResultSampleSize() const;

    //! Retrieve the random number generator seed.
    unsigned long getRNGSeed() const;

    //! Save the RCT to a file.
    int saveToFile(const char* fileName) const;

    //! Set the coverage parameter.
    bool setCoverageParameter(const float& coverageParameter);

    //! Set the verbosity of the RCT.
    void setVerbosity(const int& verbosity);

protected:
    //! Resets the query.
    void resetQuery();

    //! Returns the distance of an item from the query.
    float computeDistFromQuery(int itemIndex);

    //! Build an RCT on data items.
    void doBuild();

    //! Performs an exact range query.
    int doFindAllInRange(float limit, int sampleLevel);

    //! Performs an approximate range query.
    int doFindMostInRange(float limit, int sampleLevel, float scaleFactor);

    //! Performs an approximate nearest-neighbor query.
    int doFindNear(int howMany, int sampleLevel, float scaleFactor);

    //! Performs an exact nearest-neighbor query.
    int doFindNearest(int howMany, int sampleLevel);
    
    //! Partial quicksort.
    int partialQuickSort(int howMany, float* distList, int* indexList, int rangeFirst, int rangeLast);

    //! Print statistics related to the RCT construction.
    void printStats() const;

    //! Quicksort.
    void quickSort(float* distList, int* indexList, int rangeFirst, int rangeLast);

    //! Reserve storage for the RCT and its data.
    void reserveStorage();

    //! Accept a new query item.
    void setNewQuery(DistData* query);

    //! Setup random leveling.
    void setupLevels(int numItems, int numParents);

};

#endif /* __RCT_H */
