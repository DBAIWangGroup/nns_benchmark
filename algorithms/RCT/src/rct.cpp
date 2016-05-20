#include "rct.h"

/*!
 * Sets the sample rate.
 *
 * @note Must be called before construction.
 *
 * @param sampleRate The desired sample rate.
 */
void RCT::setSampleRate(const float& sampleRate) {
    (*this).sampleRate = sampleRate;
}

/*!
 * Constructor using seed for random number generator initialization.
 */
RCT::RCT(const unsigned long& seed) {
    data = NULL;
    size = 0;
    maxParents = 1;
    maxDegree = 0;
    avgDegree = 0.0F;
    internToExternMapping = NULL;
    levelSetSizeList = NULL;
    levels = 0;
    numNodes = 0;
    parentIndexLLList = NULL;
    parentLSizeLList = NULL;
    childIndexLLList = NULL;
    childLSizeLList = NULL;
    query = NULL;
    distFromQueryList = NULL;
    storedDistIndexList = NULL;
    numStoredDists = 0;
    numDistComps = 0UL;
    levelQuotaList = NULL;
    coverageParameter = 1.0f;
    queryResultIndexList = NULL;
    queryResultDistList = NULL;
    visitedNodeIndexList = NULL;
    tempResultIndexList = NULL;
    tempResultDistList = NULL;
    queryResultSize = 0;
    queryResultSampleSize = 0;
    verbosity = 0;
		coverageParameter = 1.0f;
		sampleRate = 2.0f;
    buildScaleFactor = RCT_UNKNOWN_;
    rand.seed(seed);
    (*this).seed = seed;
}

/*!
 * The destructor releases memory allocated by the RCT prior to its
 * destruction.
 */
RCT::~RCT() {
    int i;
    int lvl;
    int tempLength = 0;
    data = NULL;
    if (internToExternMapping != NULL) {
        delete [] internToExternMapping;
        internToExternMapping = NULL;
    }

    if (parentIndexLLList != NULL) {
        for (lvl = 0; lvl <= levels; lvl++) {
            if (parentIndexLLList[lvl] != NULL) {
                tempLength = levelSetSizeList[lvl];

                for (i = 0; i < tempLength; i++) {
                    if (parentIndexLLList[lvl][i] != NULL) {
                        delete [] parentIndexLLList[lvl][i];
                        parentIndexLLList[lvl][i] = NULL;
                    }
                }

                delete [] parentIndexLLList[lvl];
                parentIndexLLList[lvl] = NULL;
            }
        }

        delete [] parentIndexLLList;
        parentIndexLLList = NULL;
    }

    if (parentLSizeLList != NULL) {
        for (lvl = 0; lvl <= levels; lvl++) {
            if (parentLSizeLList[lvl] != NULL) {
                delete [] parentLSizeLList[lvl];
                parentLSizeLList[lvl] = NULL;
            }
        }

        delete [] parentLSizeLList;
        parentLSizeLList = NULL;
    }

    if (childIndexLLList != NULL) {
        for (lvl = 0; lvl <= levels; lvl++) {
            if (childIndexLLList[lvl] != NULL) {
                tempLength = levelSetSizeList[lvl];

                for (i = 0; i < tempLength; i++) {
                    if (childIndexLLList[lvl][i] != NULL) {
                        delete [] childIndexLLList[lvl][i];
                        childIndexLLList[lvl][i] = NULL;
                    }
                }

                delete [] childIndexLLList[lvl];
                childIndexLLList[lvl] = NULL;
            }
        }

        delete [] childIndexLLList;
        childIndexLLList = NULL;
    }

    if (childLSizeLList != NULL) {
        for (lvl = 0; lvl <= levels; lvl++) {
            if (childLSizeLList[lvl] != NULL) {
                delete [] childLSizeLList[lvl];
                childLSizeLList[lvl] = NULL;
            }
        }

        delete [] childLSizeLList;
        childLSizeLList = NULL;
    }

    if (levelSetSizeList != NULL) {
        delete [] levelSetSizeList;
        levelSetSizeList = NULL;
    }

    query = NULL;

    if (distFromQueryList != NULL) {
        delete [] distFromQueryList;
        distFromQueryList = NULL;
    }

    if (storedDistIndexList != NULL) {
        delete [] storedDistIndexList;
        storedDistIndexList = NULL;
    }

    if (levelQuotaList != NULL) {
        delete [] levelQuotaList;
        levelQuotaList = NULL;
    }

    if (queryResultIndexList != NULL) {
        delete [] queryResultIndexList;
        queryResultIndexList = NULL;
    }

    if (queryResultDistList != NULL) {
        delete [] queryResultDistList;
        queryResultDistList = NULL;
    }

    if (tempResultIndexList != NULL) {
        delete [] tempResultIndexList;
        tempResultIndexList = NULL;
    }

    if (tempResultDistList != NULL) {
        delete [] tempResultDistList;
        tempResultDistList = NULL;
    }

    if (visitedNodeIndexList != NULL) {
        delete [] visitedNodeIndexList;
        visitedNodeIndexList = NULL;
    }
}

/*!
 * Constructs the RCT from an array of data items. The maximum number of
 * parents per node (default <em>1</em>) and the scale factor used during
 * construction (default <em>1.0</em>) can be supplied as optional arguments.
 *
 * @note If a negative value of <em>scaleFactor</em> is chosen, parents will
 *       be determined using exact nearest-neighbor search. This is quite
 *       expensive, but guaranteed to produce a well-formed RCT.
 *
 * @param inputData An array of data items to build the RCT on.
 * @param numItems The number of item in the data array that should be used.
 * @param scaleFactor The scale factor used during construction.
 * @param nunParents The maximum number of parents allowed per node.
 * @return The number of items in the constructed RCT.
 */
int RCT::build(DistData** inputData, const int& numItems, const float& scaleFactor, const int& numParents) {
    int i = 0;
    int loc = 0;
    int temp = 0;

    // If the data set is empty, then abort.
    if ((numItems <= 0) || (inputData == NULL)) {
        if (verbosity > 0) {
            if (numItems == 1) {
                cerr << "ERROR (from build): data set has only 1 item." << endl;
            } else {
                cerr << "ERROR (from build): empty data set." << endl;
            }
        }
        return 0;
    }
    if (verbosity >= 2) {
        cout << "Building RCT from data array..." << endl;
    }

    data = inputData;

    // Set up level sizes.
    setupLevels(numItems, numParents);

    // Reserve RCT storage, and set up tree parameters.
    // As a result of this operation, the RCT size, number of levels,
    // etc, are set.    
    reserveStorage();

    // Randomly assign data items to RCT levels.
    for (i = 0; i < size; i++) {
        internToExternMapping[i] = i;
    }
    for (i = size - 1; i >= 0; i--) {
        loc = rand.integer() % (i + 1);
        temp = internToExternMapping[loc];
        internToExternMapping[loc] = internToExternMapping[i];
        internToExternMapping[i] = temp;
    }

    // Build the RCT structure.
    numDistComps = 0UL;
    buildScaleFactor = scaleFactor;
    doBuild();

    if (verbosity >= 2) {
        printStats();
    }
    
    return size;
}

/*!
 * Loads a previously-computed RCT from the specified file. The original data
 * set must also be provided (as well as the number of items in the data set).
 *
 * @note The extension ".rctf" is automatically appended to the file name.
 *
 * @return If successful, the number of RCT items is returned. Otherwise, zero
 *         is returned.
 */
int RCT::build(const char* fileName, DistData** inputData, const int& numItems) {
    int i = 0;
    int j = 0;
    int lvl = 0;
    int loc = 0;
    int inLevel = 0;
    int temp = 0;
    int inSize = 0;
    int inLevels = 0;
    int inNumNodes = 0;
    int inMaxParents = 0;
    int inMaxDegree = 0;
    float inAvgDegree = 0.0f;
    float inCoverageParameter = 0.0f;
    float inBuildScaleFactor = 0.0f;
    int numChildren = 0;
    int* childList = NULL;
    ifstream inFile;
    int levelSetSize = 0;

    // If the data set is empty, then abort.
    if ((fileName == NULL) || (numItems <= 0) || (inputData == NULL)) {
        if (verbosity > 0) {
            if (numItems == 1) {
                cerr << "ERROR (from build): data set has only 1 item." << endl;
            } else {
                cerr << "ERROR (from build): empty data set or filename.";
                cerr << endl;
            }

        }
        return 0;
    }

    if (verbosity >= 2) {
        cout << "Loading RCT from file '" << fileName << ".rctf' ..." << endl;
    }
    data = inputData;

    // Open the file containing the RCT. If we fail to open the file, abort.
    ostringstream fullFileName;
    fullFileName << fileName << ".rctf";
    inFile.open(fullFileName.str().c_str(), ios::in);
    if (!inFile.is_open()) {
        if (verbosity > 0) {
            cerr << "ERROR (from build): file '" << fullFileName.str();
            cerr << "' could not be opened." << endl;
        }
        return 0;
    }

    // Skip two comment lines.
    string buffer;
    getline(inFile, buffer);
    assert ((buffer[0] == '%') && (buffer[1] == '%'));
    getline(inFile, buffer);
    assert ((buffer[0] == '%') && (buffer[1] == '%'));

    // Read in basic parameters.
    inFile >> inSize >> inLevels >> inNumNodes >> inMaxParents >> inMaxDegree;
    inFile >> inAvgDegree >> inCoverageParameter >> inBuildScaleFactor;
    inFile >> seed;

    // Are these parameter values what we expected? If not, then abort!
    if (inSize != numItems) {
        if (verbosity > 0) {
            cerr << "ERROR (from build): unexpected RCT parameters in file";
            cerr << " '" << fullFileName.str() << "'." << endl;
        }
        inFile.close();
        return 0;
    }
    getline(inFile, buffer);

    // Assign properties.
    size = inSize;
    levels = inLevels;
    numNodes = inNumNodes;
    maxParents = inMaxParents;
    maxDegree = inMaxDegree;
    avgDegree = inAvgDegree;
    coverageParameter = inCoverageParameter;
    buildScaleFactor = inBuildScaleFactor;

    // Skip another comment line.
    getline(inFile, buffer);
    assert ((buffer[0] == '%') && (buffer[1] == '%'));

    // Fetch the level set sizes.
    levelSetSizeList = new int [levels + 1];
    levelQuotaList = new int [levels + 1];
    for (lvl = 0; lvl <= levels; lvl++) {
        inFile >> levelSetSizeList[lvl];
        levelQuotaList[lvl] = 0;
    }
    getline(inFile, buffer);

    // Reserve RCT storage. After this operation, the expected RCT size,
    // number of levels, etc, are set.
    reserveStorage();

    // Skip yet another comment line.
    getline(inFile, buffer);
    assert ((buffer[0] == '%') && (buffer[1] == '%'));

    // Read in information for each node: level set, internal index,
    // external index, number of children, and indices of children.
    // Build the list of children, if any exist.
    for (lvl = levels; lvl >= 0; lvl--) {
        levelSetSize = levelSetSizeList[lvl];
        for (i = 0; i < levelSetSize; i++) {
            inFile >> inLevel >> loc;
            if ((loc != i) || (lvl != inLevel)) {
                if (verbosity > 0) {
                    cerr << "ERROR (from build): invalid entry in file '";
                    cerr << fileName << ".rctf'." << endl;
                }
                inFile.close();
                return 0;
            }
            inFile >> internToExternMapping[i] >> numChildren;
            if (numChildren > 0) {
                childList = new int [numChildren];
            } else {
                childList = NULL;
            }
            childIndexLLList[lvl][i] = childList;
            childLSizeLList[lvl][i] = numChildren;
            for (j = 0; j < numChildren; j++) {
                inFile >> childList[j];
            }
            getline(inFile, buffer);
        }
    }
    return size;
}

/*!
 * Perform an exact range query for the specified item. The upper limit on the
 * query-to-item distance must be supplied. The search is relative to the
 * random level <em>sampleLevel</em> (default is <em>0</em>, i.e. the complete
 * data set).
 * 
 * @param query The query location.
 * @param limit The range limit.
 * @param sampleLevel The sample level with respect to which the query is
 *                    performed.
 * 
 * @return The number of elements actually found.
 *
 * @note The query result can be obtained via calls to the following methods:
 *       <code>getResultAcc</code>, <code>getResultDists</code>,
 *       <code>getResultDistComps</code>, <code>getResultIndices</code> and
 *       <code>getResultNumFound</code>. The result items are sorted in
 *       increasing order of their distances to the query.
 */
int RCT::findAllInRange(DistData* query, const float& limit, const int& sampleLevel) {
    queryResultSize = 0;
    queryResultSampleSize = 0;
    numDistComps = 0UL;
    if ((size <= 0)
            || (query == NULL)
            || (limit < 0.0F)
            || (sampleLevel < 0)
            || ((sampleLevel >= levels) && (size > 1))) {
        if (verbosity > 0) {
            cerr << "ERROR (from findAllInRange): invalid argument(s)." << endl;
        }
        return 0;
    }
    setNewQuery(query);
    return doFindAllInRange(limit, sampleLevel);
}

/*!
 * Perform an approximate range query for the specified item. The upper limit on
 * the query-to-item distance must be supplied. The search is relative to the
 * random level <em>sampleLevel</em> (default is <em>0</em>, i.e. the complete
 * data set). The method also makes use of a parameter (<code>scaleFactor
 * </code>) that influences the trade-off between time and accuracy.
 *
 * @param scaleFactor Additional search efforts.
 * @param query The query location.
 * @param limit The range limit.
 * @param sampleLevel The sample level with respect to which the query is
 *                    performed.
 *
 * @return The number of elements actually found.
 *
 * @note The query result can be obtained via calls to the following methods:
 *       <code>getResultAcc</code>, <code>getResultDists</code>,
 *       <code>getResultDistComps</code>, <code>getResultIndices</code> and
 *       <code>getResultNumFound</code>. The result items are sorted in
 *       increasing order of their distances to the query.
 */
int RCT::findMostInRange(DistData* query, const float& limit, const float& scaleFactor, const int& sampleLevel) {
    queryResultSize = 0;
    queryResultSampleSize = 0;
    numDistComps = 0UL;
    if ((size <= 0)
            || (query == NULL)
            || (limit < 0.0F)
            || (sampleLevel < 0)
            || ((sampleLevel >= levels) && (size > 1))
            || (scaleFactor <= 0.0F)) {
        if (verbosity > 0) {
            cerr << "ERROR (from findMostInRange): invalid argument(s)." << endl;
        }
        return 0;
    }
    setNewQuery(query);
    return doFindMostInRange(limit, sampleLevel, scaleFactor);
}

/*!
 * Perform an approximate nearest-neighbor query for the specified item. The 
 * number of desired nearest neighbors <code>howMany</code> (default <em>1</em>)
 * can be specified. The search is relative to the random level 
 * <em>sampleLevel</em> (default is <em>0</em>, i.e. the complete
 * data set). The method also makes use of a parameter (<code>scaleFactor
 * </code>) that influences the trade-off between time and accuracy.
 *
 * @param scaleFactor Additional search efforts.
 * @param query The query location.
 * @param howMany The desired number of neighbors.
 * @param sampleLevel The sample level with respect to which the query is
 *                    performed.
 *
 * @return The number of elements actually found.
 *
 * @note The query result can be obtained via calls to the following methods:
 *       <code>getResultAcc</code>, <code>getResultDists</code>,
 *       <code>getResultDistComps</code>, <code>getResultIndices</code> and
 *       <code>getResultNumFound</code>. The result items are sorted in
 *       increasing order of their distances to the query.
 */
int RCT::findNear(DistData* query, const int& howMany, const float& scaleFactor, const int& sampleLevel) {
    queryResultSize = 0;
    queryResultSampleSize = 0;
    numDistComps = 0UL;
    if ((size <= 0)
            || (query == NULL)
            || (howMany <= 0)
            || (sampleLevel < 0)
            || ((sampleLevel >= levels) && (size > 1))
            || (scaleFactor <= 0.0F)) {
        if (verbosity > 0) {
            cerr << "ERROR (from findNear): invalid argument(s)." << endl;
        }
        return 0;
    }
    setNewQuery(query);
    return doFindNear(howMany, sampleLevel, scaleFactor);
}

/*!
 * Perform an exact nearest-neighbor query for the specified item. The
 * number of desired nearest neighbors <code>howMany</code> (default <em>1</em>)
 * can be specified. The search is relative to the random level
 * <em>sampleLevel</em> (default is <em>0</em>, i.e. the complete
 * data set).
 *
 * @param query The query location.
 * @param howMany The desired number of neighbors.
 * @param sampleLevel The sample level with respect to which the query is
 *                    performed.
 *
 * @return The number of elements actually found.
 *
 * @note The query result can be obtained via calls to the following methods:
 *       <code>getResultAcc</code>, <code>getResultDists</code>,
 *       <code>getResultDistComps</code>, <code>getResultIndices</code> and
 *       <code>getResultNumFound</code>. The result items are sorted in
 *       increasing order of their distances to the query.
 */
int RCT::findNearest(DistData* query, const int& howMany, const int& sampleLevel) {
    queryResultSize = 0;
    queryResultSampleSize = 0;
    numDistComps = 0UL;
    if ((size <= 0)
            || (query == NULL)
            || (howMany <= 0)
            || (sampleLevel < 0)
            || ((sampleLevel >= levels) && (size > 1))) {
        if (verbosity > 0) {
            cerr << "ERROR (from findNearest): invalid argument(s)." << endl;
        }
        return 0;
    }
    setNewQuery(query);
    return doFindNearest(howMany, sampleLevel);
}

/*!
 * Retrieve the average degree of a node in the RCT. This value is roughly
 * equal to <em>sampleRate</em>.
 *
 * @return The average degree of a node in the RCT.
 */
float RCT::getAvgDegree() const {
    return avgDegree;
}

/*!
 * Retrieve the scale factor used during the consturction of the RCT.
 *
 * @return The scale factor used during RCT construction.
 */
float RCT::getBuildScaleFactor() const {
    return buildScaleFactor;
}

/*!
 * Retrieve the coverage parameter set for the RCT.
 *
 * @return The coverage parameter of the RCT.
 */
float RCT::getCoverageParameter() const {
    return coverageParameter;
}

/*!
 * Retrieve a pointer to the data array used to assemble the
 * RCT.
 *
 * @note Since the random leveling process only permutes the
 *       data items internally, be aware that only the item
 *       indices retrieved by the <code>getResultIndices</code>
 *       function refer to external (original) indices. All
 *       internally used indices reffer to the permuted
 *       ordering of data items.
 * 
 * @return A pointer to the data items in the RCT.
 */
DistData** RCT::getData() {
    return data;
}

/*!
 * Fills the supplied list with the mapping from external item indices to
 * internal RCT indices.
 *
 * @param result An array to store the mapping in.
 * @param capacity The capacity of the target array.
 *
 * @return If successful, the number of RCT items is returned. Otherwise, zero
 *         is returned.
 */
int RCT::getExternToInternMapping(int* result, int capacity) const {
    int i;
    if ((result == NULL) || (capacity < size)) {
        if (verbosity > 0) {
            cerr << "ERROR (from getExternToInternMapping): result list ";
            cerr << "capacity is too small." << endl;
        }
        return 0;
    }
    for (i = 0; i < size; i++) {
        result[internToExternMapping[i]] = i;
    }
    return size;
}

/*!
 * Fills the supplied list with the mapping from internal RCT item indices to
 * external indices.
 *
 * @param result An array to store the mapping in.
 * @param capacity The capacity of the target array.
 *
 * @return If successful, the number of RCT items is returned. Otherwise, zero
 *         is returned.
 */
int RCT::getInternToExternMapping(int* result, int capacity) const {
    int i;
    if ((result == NULL) || (capacity < size)) {
        if (verbosity > 0) {
            cerr << "ERROR (from getInternToExternMapping): result list ";
            cerr << "capacity is too small." << endl;
        }
        return 0;
    }
    for (i = 0; i < size; i++) {
        result[i] = internToExternMapping[i];
    }
    return size;
}

/*!
 * Fills the supplied list with the RCT level set sizes, from smallest to 
 * largest. The result does not include the <em>sample</em> consisting solely of
 * the virtual RCT root item.
 * 
 * @param result An array to hold the level set sizes.
 * @param capacity The capacity of the target array.
 * 
 * @return If successful, the number of RCT levels is returned (excluding that
 *         of the root). If unsuccessful, zero is returned.
 */
int RCT::getLevelSetSizes(int* result, int capacity) const {
    int lvl;
    if ((result == NULL) || (capacity < levels)) {
        if (verbosity > 0) {
            cerr << "ERROR (from getLevelSetSizes): result list capacity is ";
            cerr << "too small." << endl;
        }
        return 0;
    }
    for (lvl = 0; lvl < levels; lvl++) {
        result[lvl] = levelSetSizeList[lvl];
    }
    return levels;
}

/*!
 * Retrieve the maximum node degree occurring in the RCT.
 *
 * @return The degree of the node with the largest number of children.
 */
int RCT::getMaxDegree() const {
    return maxDegree;
}

/*!
 * Fills the supplied list with the mapping from external item indices to
 * internal RCT level sets.
 * 
 * @param result An array to hold the item heights.
 * @param capacity The capacity of the target array.
 * 
 * @return If successful, the number of RCT items is returned. If unsuccessful,
 *         zero is returned.
 */
int RCT::getMaxLevelAssignment(int* result, int capacity) const {
    int i;
    int lvl;
    if ((result == NULL) || (capacity < size)) {
        if (verbosity > 0) {
            cerr << "ERROR (from getMaxLevelAssignment): result list capacity ";
            cerr << "is too small." << endl;
        }
        return 0;
    }
    for (lvl = 0; lvl < levels; lvl++) {
        for (i = levelSetSizeList[lvl + 1]; i < levelSetSizeList[lvl]; i++) {
            result[internToExternMapping[i]] = lvl;
        }
    }
    result[internToExternMapping[0]] = levels;
    return size;
}

/*!
 * Retrieve the maximum number of parents a node in the RCT is allowed to
 * have.
 *
 * @return The maxmimum number of allowed parents per node.
 */
int RCT::getMaxParents() const {
    return maxParents;
}

/*!
 * Retrieve the number of data items stored in the RCT.
 *
 * @return The number of data items the RCT is built on.
 */
int RCT::getNumItems() const {
    return size;
}

/*!
 * Retrieve the number of levels in the RCT.
 *
 * @note Since the RCT is built on level sets whose membership in items
 *       is purely randomized one can observe variance in height when
 *       rebuilding an RCT on the same data with a different random leveling
 *       (using different seeds for example).
 */
int RCT::getNumLevels() const {
    return levels;
}

/*!
 * Retrieve the number of nodes in the RCT.
 *
 * @note Although the model of the RCT relies on using copies of nodes
 *       in different level sets, the actual implementation does not do this.
 *       The number of nodes should therefore be seen as a symbolic property
 *       of the random leveling and not as a contributor to the search
 *       or construction time complexity.
 *
 * @return The number of nodes in the RCt.
 */
int RCT::getNumNodes() const {
    return numNodes;
}

/*!
 * Computes the recall accuracy of the most recent query result. A list of the
 * exact distances must be provided, sorted from smallest to largest. The
 * number of exact distances provided determines the size of the neighbourhood
 * within which the accuracy is assessed.
 *
 * @note The list must contain at least as many entries as the number of items
 *       found in the query result.
 *
 * @return If unsuccessful, a negative value is returned.
 */
float RCT::getResultAcc(float* exactDistList, int howMany) const {
    int i;
    int loc = 0;
    if ((exactDistList == NULL) || (howMany < queryResultSize)) {
        if (verbosity > 0) {
            cerr << "ERROR (from getResultAcc): exact distance list is too ";
            cerr << "small." << endl;
        }
        return RCT_UNKNOWN_;
    }
    for (i = 0; i < howMany; i++) {
        if ((loc < queryResultSize)
                && (queryResultDistList[loc] <= exactDistList[i])) {
            loc++;
        }
    }
    return ((float)loc) / howMany;
}

/*!
 * Fills the supplied list with the query-to-neighbour distances found in the
 * most recent RCT query.
 * 
 * @param result An array to store the distance values.
 * @param capacity The capacity of the target array.
 * 
 * @return If successful, the number of items found is returned. Otherwise,
 *         zero is returned.
 */
int RCT::getResultDists(float* result, int capacity) const {
    int i;
    if ((result == NULL) || (capacity < queryResultSize)) {
        if (verbosity > 0) {
            cerr << "ERROR (from getResultDists): result list capacity is too ";
            cerr << "small." << endl;
        }
        return 0;
    }
    for (i = 0; i < queryResultSize; i++) {
        result[i] = queryResultDistList[i];
    }
    return queryResultSize;
}

/*!
 * Retrieve the number of distance comparisons performed during the most
 * recent operation.
 *
 * @return The number of distance comparisons.
 */
unsigned long RCT::getResultDistComps() const {
    return numDistComps;
}

/*!
 * Fills the supplied list with the (external) indices of the items found in
 * the most recent RCT query.
 *
 * @param result An array to store the item indices.
 * @param capacity The capacity of the target array.
 *
 * @return If successful, the number of items found is returned. Otherwise,
 *         zero is returned.
 */
int RCT::getResultIndices(int* result, int capacity) const {
    int i;
    if ((result == NULL) || (capacity < queryResultSize)) {
        if (verbosity > 0) {
            cerr << "ERROR (from getResultIndices): result list capacity is ";
            cerr << "too small." << endl;
        }
        return 0;
    }
    for (i = 0; i < queryResultSize; i++) {
        result[i] = internToExternMapping[queryResultIndexList[i]];
    }
    return queryResultSize;
}

/*!
 * Returns the number of items found in the most recent query.
 *
 * @return Number of results found in the most recent query.
 */
int RCT::getResultNumFound() const {
    return queryResultSize;
}

/*!
 * Returns the sample size used in the most recent query.
 *
 * @return Sample size used in the most recent query.
 */
int RCT::getResultSampleSize() const {
    return queryResultSampleSize;
}

/*!
 * Retrieve the seed value used to initialize the random number generator.
 */
unsigned long RCT::getRNGSeed() const {
    return seed;
}

/*!
 * Resets the current query object to <em>NULL</em>. This has the effect of
 * clearing any saved distances - subsequent <code>findNear</code> and
 * <code>findNearest</code> operations would be forced to compute
 * all needed distances from scratch.
 */
void RCT::resetQuery() {
    setNewQuery(NULL);
}

/*!
 * Save the RCT to the specified file. The extension ".rctf" is automatically 
 * appended to the file name.
 *
 * @param fileName The file name to save the RCT under.
 * @return If successful, the number of RCT items is returned. Otherwise,
 *         zero is returned.
 */
int RCT::saveToFile(const char* fileName) const {
    int i;
    int j;
    int lvl;
    int numChildren = 0;
    int* childList = NULL;
    ofstream outFile;
    int levelSetSize = 0;

    // If the RCT has not yet been built, abort.
    if (size <= 0) {
        return 0;
    }

    // Open the file for writing.
    // If this fails, then abort.
    if (fileName == NULL) {
        if (verbosity > 0) {
            cerr << "ERROR (from saveToFile): output file name is NULL." << endl;
        }
        return 0;
    }

    // Attach extension '.rctf'.
    ostringstream fullFileName;
    fullFileName << fileName << ".rctf";
    
    // Try to open the output file.
    outFile.open(fullFileName.str().c_str(), ios::out);
    if (!outFile.is_open()) {
        if (verbosity > 0) {
            cerr << "ERROR (from saveToFile): file '" << fullFileName.str();
            cerr << "' could not be opened." << endl;
        }
        return 0;
    }

    // Begin writing to the output file. First, write a comment identifying the
    // RCT version and the output file name.
    outFile << "%% RCT " << RCT_VERSION_ <<  ' ' << fileName << endl;
    
    // Write the main RCT parameters.
    outFile << "%% size levels numNodes maxParents maxDegree avgDegree ";
    outFile << "coverageParameter buildScaleFactor seed" << endl;
    outFile << size << ' ' << levels << ' ' << numNodes << ' ';
    outFile << maxParents << ' ' << maxDegree << ' ' << avgDegree;
    outFile << ' ' << coverageParameter << ' ' << buildScaleFactor;
    outFile << ' ' << seed << endl;

    // Write the level sizes.
    outFile << "%% level set sizes:" << endl;
    for (lvl = 0; lvl < levels; lvl++) {
        outFile << levelSetSizeList[lvl] << ' ';
    }
    outFile << 1 << endl;

    // For each item at each level, write out:
    //   its level,
    //   its index,
    //   the index of the item in the original input list,
    //   the number of children of the item,
    //   and a list of the indices of the children.
    outFile << "%% level nodeID origItemID numChildren c_0 c_1 ..." << endl;
    for (lvl = levels; lvl >= 0; lvl--) {
        levelSetSize = levelSetSizeList[lvl];
        for (i = 0; i < levelSetSize; i++) {
            numChildren = childLSizeLList[lvl][i];
            childList = childIndexLLList[lvl][i];
            outFile << lvl << ' ' << i << ' ' << internToExternMapping[i];
            outFile << ' ' << numChildren;
            for (j = 0; j < numChildren; j++) {
                outFile << ' ' << childList[j];
            }
            outFile << endl;
        }
    }
    outFile.close();
    return size;
}

/*!
 * Sets the coverage parameter to a specific value.
 * 
 * @param coverageParameter The desired value of the coverage parameter.
 * @return If the coverage parameter was changes <em>true</em> is returned.
 *         Otherwise, <em>false</em> is returned.
 */
bool RCT::setCoverageParameter(const float& coverageParameter) {
    if ((*this).coverageParameter == coverageParameter) {
        return false;
    }
    (*this).coverageParameter = coverageParameter;
    return true;
}

/*!
 * Sets the verbosity level for messages. Verbosity of <em>zero</em> or less:
 * no messages produced. Verbosity of <em>1</em>: error messages only.
 * Verbosity of <em>2</em>: error and progress messages only. Verbosity of
 * <em>3</em> or more: error, progress, and debug messages reported.
 */
void RCT::setVerbosity(const int& verbosity) {
    if (verbosity <= 0) {
        (*this).avgDegree = 0;
    } else if (verbosity >= 3) {
        (*this).verbosity = 3;
    } else {
        (*this).verbosity = verbosity;
    }
}

/*!
 * Returns the distance from the current query object to the specified data
 * object. If the distance has already been computed and stored, the stored
 * distance is returned. Otherwise, the distance is computed and stored
 * before returning it.
 *
 * @param itemIndex The internal index of a data item.
 * @return The distance from the current query to that item.
 */
float RCT::computeDistFromQuery(int itemIndex) {
    if (distFromQueryList[itemIndex] == RCT_UNKNOWN_) {
        distFromQueryList[itemIndex]
                = query->distanceTo(data[internToExternMapping[itemIndex]]);
        storedDistIndexList[numStoredDists] = itemIndex;
        numStoredDists++;
        numDistComps++;
    }
    return distFromQueryList[itemIndex];
}

/*!
 * Builds an RCT on items in the first locations of the scrambled data array.
 */
void RCT::doBuild() {
    int i = 0;
    int j = 0;
    int lvl = 0;
    int numLowerItems = 0;
    int numUpperItems = 0;
    int parent = 0;
    int child = 0;
    int childLSize = 0;
    int offset = 0;
    long int totalDegree = 0L;

    // Build the top level of the RCT as a special case.
    // Treat the first array item as the root.
    parentLSizeLList[levels][0] = 0;
    parentIndexLLList[levels][0] = NULL;

    // Explicitly connect all other items as children of the root,
    //   if they exist.
    // Don't bother sorting the children according to distance from
    //   the root.
    numLowerItems = levelSetSizeList[levels - 1];
    childLSizeLList[levels][0] = numLowerItems;
    childIndexLLList[levels][0] = new int [numLowerItems];
    maxDegree = numLowerItems;
    totalDegree = (long int)numLowerItems;

    for (i = 0; i < numLowerItems; i++) {
        childIndexLLList[levels][0][i] = i;
    }

    if (verbosity >= 2) {
        printf("RCT root level constructed.\n");
        fflush(NULL);
    }

    for (lvl = levels - 2; lvl >= 0; lvl--) {
        numUpperItems = levelSetSizeList[lvl + 1];
        numLowerItems = levelSetSizeList[lvl];

        // We now want to connect the bottom level of the
        //   current RCT (level lvl+1) to the items at
        //   this level (level lvl).

        // For each item at this level, generate a set of
        //   parents from the bottom level of the current RCT.
        // Also, temporarily store (in "childLSizeLList") the number of times
        //   each node is requested as a parent.
        for (child = 0; child < numLowerItems; child++) {
            if ((child % 5000 == 4999) && (verbosity >= 2)) {
                printf("Inserting item %d (out of %d) at level %d...\n",
                        child + 1, size, lvl);
                fflush(NULL);
            }

            // Find some parents for the current child.
            // If only one parent is requested and the child has a copy
            //   at the level above, then just choose it directly.
            // Otherwise, do a search.

            if ((child < numUpperItems) && (maxParents == 1)) {
                queryResultSize = 1;
                queryResultIndexList[0] = child;
            } else {
                setNewQuery(data[internToExternMapping[child]]);

                if (buildScaleFactor <= 0.0F) {
                    doFindNearest(maxParents, lvl + 1);
                } else {
                    doFindNear(maxParents, lvl + 1, buildScaleFactor);
                }
            }

            // Connect links from child to parents.
            // If a copy of the child also exists at the upper level,
            //   then make sure that it is listed as the first parent.

            parentLSizeLList[lvl][child] = queryResultSize;
            parentIndexLLList[lvl][child] = new int [maxParents];

            if ((child < numUpperItems) && (queryResultIndexList[0] != child)) {
                // The first query result should have been a copy of the
                //   child, but wasn't.
                // Repair this situation by explicitly placing a copy of the child
                //   at the head of the list, and shifting the remaining query
                //   result elements to accommodate the child copy.

                offset = 1;
                parentIndexLLList[lvl][child][0] = child;
                childLSizeLList[lvl + 1][child]++;
            } else {
                // Either the query result contains a copy of the child at its
                //   head, or the child isn't supposed to appear in the
                //   query result anyway.

                offset = 0;
                parentIndexLLList[lvl][child][0] = queryResultIndexList[0];
                childLSizeLList[lvl + 1][queryResultIndexList[0]]++;
            }

            for (i = 1; i < queryResultSize; i++) {
                // Apply an offset shift only until a copy of the child is found
                //   (one may not necessarily be found).
                // This is to avoid picking up this copy more than once.

                if (queryResultIndexList[i] == child) {
                    offset = 0;
                } else {
                    parentIndexLLList[lvl][child][i]
                            = queryResultIndexList[i - offset];
                    childLSizeLList[lvl + 1][queryResultIndexList[i - offset]]++;
                }
            }
        }

        // For each parent, reserve storage for its child lists.

        for (parent = 0; parent < numUpperItems; parent++) {
            childLSize = childLSizeLList[lvl + 1][parent];

            if (childLSize > 0) {
                childIndexLLList[lvl + 1][parent]
                        = new int [childLSizeLList[lvl + 1][parent]];
                childLSizeLList[lvl + 1][parent] = 0;

                totalDegree += (long int)childLSize;

                if (childLSize > maxDegree) {
                    maxDegree = childLSize;
                }
            }
        }

        // Construct child lists for each of the parents,
        //   by reversing the child-to-parent edges.
        // Since the child-to-parent edges are no longer needed,
        //   delete them.

        for (child = 0; child < numLowerItems; child++) {
            for (i = parentLSizeLList[lvl][child] - 1; i >= 0; i--) {
                parent = parentIndexLLList[lvl][child][i];
                j = childLSizeLList[lvl + 1][parent];
                childIndexLLList[lvl + 1][parent][j] = child;
                childLSizeLList[lvl + 1][parent]++;
            }

            if (parentLSizeLList[lvl][child] > 0) {
                delete [] parentIndexLLList[lvl][child];
                parentIndexLLList[lvl][child] = NULL;
                parentLSizeLList[lvl][child] = 0;
            }
        }

        // The RCT has grown by one level.
        if (verbosity >= 2) {
					cout << "RCT level " << lvl << " constructed." << endl;
        }
    }

    avgDegree = (float)(((double)totalDegree) / (numNodes - size));
}

/*!
 * Performs an exact range query from the current query object, with respect to
 * a subset of the items. The subset consists of all items at the indicated
 * sample level and higher. The upper limit on the query-to-item distance is
 * <em>limit</em>; the number of neighbours actually found is returned.
 *
 * @param limit The limit on the query-to-neighbor distance.
 * @param sampleLevel The sample level which is searched.
 *
 * @return The number of neighbors found.
 */
int RCT::doFindAllInRange(float limit, int sampleLevel) {
    int i;

    // Handle the singleton case separately.
    if (size == 1) {
        queryResultDistList[0] = computeDistFromQuery(0);
        queryResultIndexList[0] = 0;
        queryResultSampleSize = 1;

        if (queryResultDistList[0] <= limit) {
            queryResultSize = 1;
        } else {
            queryResultSize = 0;
        }

        return queryResultSize;
    }

    queryResultSampleSize = levelSetSizeList[sampleLevel];

    // Compute distances from the current query to all items.

    for (i = 0; i < queryResultSampleSize; i++) {
        queryResultDistList[i] = computeDistFromQuery(i);
        queryResultIndexList[i] = i;
    }

    // Sort the items by distances, returning the number of 
    //   elements actually found.

    quickSort
            (queryResultDistList,
            queryResultIndexList,
            0,
            queryResultSampleSize - 1);

    // Report only those items whose distances fall within the limit.

    i = 0;

    while ((i < queryResultSize) && (queryResultDistList[i] <= limit)) {
        i++;
    }

    queryResultSize = i;

    return queryResultSize;
}

/*!
 * Performs an approximate range query from the current query object, with
 * respect to a subset of the items. The subset consists of all items at the
 * indicated sample level and higher. The upper limit on the query-to-item
 * distance must be supplied. The number of elements actually found is returned.
 *
 * The results are stored in the RCT query result lists. The parameter
 * <em>scaleFactor</em> influences the tradeoff between speed and accuracy.
 *
 * @param limit The limit of query-to-neighbor distance.
 * @param sampleLevel which is searched.
 * @param scaleFactor The time/accuracy trade-off factor.
 *
 * @return Number of neighbors found.
 */
int RCT::doFindMostInRange(float limit, int sampleLevel, float scaleFactor) {
    int i;
    int j;
    int lvl;
    int child = 0;
    int minNeighbors = 0;
    int nodeIndex = 0;
    int numChildren = 0;
    int numFound = 0;
    int numRetained = 0;
    int* childList = NULL;

    // Handle the singleton case separately.

    if (size == 1) {
        queryResultDistList[0] = computeDistFromQuery(0);
        queryResultIndexList[0] = 0;
        queryResultSampleSize = 1;

        if (queryResultDistList[0] <= limit) {
            queryResultSize = 1;
        } else {
            queryResultSize = 0;
        }

        return queryResultSize;
    }

    // Compute the sample size for the operation.

    queryResultSampleSize = levelSetSizeList[sampleLevel];

    // Compute the minimum number of neighbours for each sample level.

    minNeighbors = (int)((scaleFactor * coverageParameter) + 0.999999F);

    // Load the root as the tentative sole member of the query result list.

    queryResultSize = 0;

    queryResultDistList[0] = computeDistFromQuery(0);
    queryResultIndexList[0] = 0;
    numRetained = 1;

    // From the root, search out other nodes to place in the query result.

    for (lvl = levels - 1; lvl >= sampleLevel; lvl--) {
        // For every node at the active level, load its children
        //   into the scratch list, and compute their distances to the query.

        numFound = 0;

        for (i = 0; i < numRetained; i++) {
            nodeIndex = queryResultIndexList[i];
            numChildren = childLSizeLList[lvl][nodeIndex];
            childList = childIndexLLList[lvl][nodeIndex];

            for (j = 0; j < numChildren; j++) {
                child = childList[j];

                if (visitedNodeIndexList[child] != TRUE) {
                    visitedNodeIndexList[child] = TRUE;
                    tempResultIndexList[numFound] = child;
                    tempResultDistList[numFound] = computeDistFromQuery(child);
                    numFound++;
                }
            }
        }

        for (i = 0; i < numFound; i++) {
            visitedNodeIndexList[tempResultIndexList[i]] = FALSE;
        }

        // Extract the closest nodes from the list of accumulated children,
        //   and keep them as the tentative parents of the query.

        quickSort(tempResultDistList, tempResultIndexList, 0, numFound - 1);

        // Determine the elements in the query result that
        //   lie within the range.

        queryResultSize = 0;

        while ((queryResultSize < numFound)
                && (tempResultDistList[queryResultSize] <= limit)) {
            queryResultIndexList[queryResultSize]
                    = tempResultIndexList[queryResultSize];
            queryResultDistList[queryResultSize]
                    = tempResultDistList[queryResultSize];
            queryResultSize++;
        }

        // Determine the number of elements to be retained at this level.

        numRetained = (int)((scaleFactor * numFound * coverageParameter) + 0.999999F);

        if (numRetained < minNeighbors) {
            numRetained = minNeighbors;
        }

        if (numRetained > numFound) {
            numRetained = numFound;
        }
    }

    childList = NULL;

    return queryResultSize;
}

/*!
 * Perform an approximate nearest-neighbor query for the specified item. The
 * number of desired nearest neighbors <code>howMany</code> must be specified.
 * The search is relative to the random level <em>sampleLevel</em>.
 *
 * @param scaleFactor Additional search efforts.
 * @param howMany The desired number of neighbors.
 * @param sampleLevel The sample level with respect to which the query is
 *                    performed.
 *
 * @return The number of elements actually found.
 */
int RCT::doFindNear(int howMany, int sampleLevel, float scaleFactor) {
    int i;
    int j;
    int lvl;
    int child = 0;
    int nodeIndex = 0;
    int numChildren = 0;
    int tempQueryResultSize = 0;
    double varQuota = 0.0F;
    int numFound = 0;
    int numRetained = 0;
    int* childList = NULL;

    // Compute quota of items to be retained at every level.
    // Rank cover tree rules.levelQuotaList
    varQuota = (double)howMany;
    for (lvl = sampleLevel; lvl < levels; lvl++) {
        levelQuotaList[lvl] = (int)((scaleFactor * varQuota * coverageParameter) + 0.999999F);
        if (levelQuotaList[lvl] < scaleFactor * coverageParameter) {
            levelQuotaList[lvl] = (int)((scaleFactor * coverageParameter) + 0.999999F);
        }
        varQuota /= sampleRate;
    }
    if (howMany > levelQuotaList[sampleLevel]) {
        levelQuotaList[sampleLevel] = howMany;
    }

    // Load the root as the tentative sole member of the query result list.
    queryResultSize = 0;
    queryResultDistList[0] = computeDistFromQuery(0);
    queryResultIndexList[0] = 0;
    numRetained = 1;
    checks = 0;
    // From the root, search out other nodes to place in the query result.
    for (lvl = levels - 1; lvl >= sampleLevel; lvl--) {
        // For every node at the active level, load its children
        //   into the scratch list, and compute their distances to the query.
        numFound = 0;

        for (i = 0; i < numRetained; i++) {
            nodeIndex = queryResultIndexList[i];
            numChildren = childLSizeLList[lvl + 1][nodeIndex];
            childList = childIndexLLList[lvl + 1][nodeIndex];

            for (j = 0; j < numChildren; j++) {
                child = childList[j];

                if (visitedNodeIndexList[child] != TRUE) {
                    visitedNodeIndexList[child] = TRUE;
                    tempResultIndexList[numFound] = child;
                    tempResultDistList[numFound] = computeDistFromQuery(child);
                    numFound++;
					checks++;
                }
            }
        }

        for (i = 0; i < numFound; i++) {
            visitedNodeIndexList[tempResultIndexList[i]] = FALSE;
        }

        // Extract the closest nodes from the list of accumulated children,
        //   and keep them as the tentative parents of the query.

        if (numFound > levelQuotaList[lvl]) {
            numRetained = levelQuotaList[lvl];
        } else {
            numRetained = numFound;
        }

        numRetained = partialQuickSort
                (numRetained,
                tempResultDistList,
                tempResultIndexList,
                0,
                numFound - 1);

        for (i = 0; i < numRetained; i++) {
            queryResultIndexList[i] = tempResultIndexList[i];
            queryResultDistList[i] = tempResultDistList[i];
        }
    }

    // Select the final number of neighbors needed.
    if (numRetained > howMany) {
        queryResultSize = howMany;
    } else {
        queryResultSize = numRetained;
    }
    childList = NULL;
    return queryResultSize;

}

/*!
 * Perform an exact nearest-neighbor query for the specified item. The
 * number of desired nearest neighbors <code>howMany</code> must be specified.
 * The search is relative to the random level <em>sampleLevel</em>.
 *
 * @param howMany The desired number of neighbors.
 * @param sampleLevel The sample level with respect to which the query is
 *                    performed.
 *
 * @return The number of elements actually found.
 */
int RCT::doFindNearest(int howMany, int sampleLevel) {
    int i;

    // Handle the singleton case separately.
    if (size == 1) {
        queryResultSize = 1;
        queryResultDistList[0] = computeDistFromQuery(0);
        queryResultIndexList[0] = 0;

        return 1;
    }

    queryResultSize = levelSetSizeList[sampleLevel];

    // Compute distances from the current query to all items.
    for (i = 0; i < queryResultSize; i++) {
        queryResultDistList[i] = computeDistFromQuery(i);
        queryResultIndexList[i] = i;
    }

		queryResultSize = partialQuickSort(howMany, queryResultDistList, 
																			 queryResultIndexList, 0, queryResultSize - 1);

    return queryResultSize;
}

/*!
 * Sorts the smallest items in the supplied list ranges, in place, according to
 * distances. A partial quicksort is used to sort only the requested number
 * of items. The smallest items are placed at the beginning of the range, in
 * increasing order of distance.
 *
 * @note The remainder of the range can become corrupted by this operation!
 *
 * @param howMany Number of nearest items sought.
 * @param distList List of item distances.
 * @param indexList List of item indices.
 * @param rangeFirst First element in range.
 * @param rangeLast Last element in range.
 *
 * @return Number of items sorted.
 */
int RCT::partialQuickSort(int howMany, float* distList, int* indexList, int rangeFirst, int rangeLast) {
    int i;
    int pivotLoc = 0;
    int pivotIndex = 0;
    float pivotDist = 0.0F;
    int tempIndex = 0;
    float tempDist = 0.0F;
    int low = 0;
    int high = 0;
    int numFound = 0;
    int numDuplicatesToReplace = 0;
    int tieBreakIndex = 0;

    // If the range is empty, or if we've been asked to sort no
    //   items, then return immediately.

    if ((rangeLast < rangeFirst) || (howMany < 1)) {
        return 0;
    }

    // If there is exactly one element, then again there is nothing
    //   that need be done.

    if (rangeLast == rangeFirst) {
        return 1;
    }

    // If the range to be sorted is small, just do an insertion sort.

    if (rangeLast - rangeFirst < 7) {
        high = rangeFirst + 1;
        tieBreakIndex = indexList[rand.integer() % (rangeLast - rangeFirst + 1)];

        // The outer while loop considers each item in turn (starting
        //   with the second item in the range), for insertion into
        //   the sorted list of items that precedes it.

        while (high <= rangeLast) {
            // Copy the next item to be inserted, as the "pivot".
            // Start the insertion tests with its immediate predecessor.

            pivotDist = distList[high];
            pivotIndex = indexList[high];
            low = high - 1;

            // Work our way down through previously-sorted items
            //   towards the start of the range.

            while (low >= rangeFirst) {
                // Compare the item to be inserted (the "pivot") with
                //   the current item.

                if (distList[low] < pivotDist) {
                    // The current item precedes the pivot in the sorted order.
                    // Break out of the loop - we have found the insertion point.

                    break;
                } else if (distList[low] > pivotDist) {
                    // The current item follows the pivot in the sorted order.
                    // Shift the current item one spot upwards, to make room
                    //   for inserting the pivot below it.

                    distList[low + 1] = distList[low];
                    indexList[low + 1] = indexList[low];
                    low--;
                } else {
                    if (indexList[low] != pivotIndex) {
                        // The items have the same sort value but are not identical.
                        // Break the tie pseudo-randomly.

                        if (
                                (
                                (tieBreakIndex >= pivotIndex)
                                &&
                                (
                                (indexList[low] < pivotIndex)
                                ||
                                (tieBreakIndex < indexList[low])
                                )
                                )
                                ||
                                (
                                (tieBreakIndex < pivotIndex)
                                &&
                                (
                                (indexList[low] < pivotIndex)
                                &&
                                (tieBreakIndex < indexList[low])
                                )
                                )
                                ) {
                            // The current item precedes the pivot in the sorted order.
                            // Break out of the loop - we have found the insertion point.

                            break;
                        } else {
                            // The current item follows the pivot in the sorted order.
                            // Shift the current item one spot upwards, to make room
                            //   for inserting the pivot below it.

                            distList[low + 1] = distList[low];
                            indexList[low + 1] = indexList[low];
                            low--;
                        }
                    } else {
                        // Oh no!
                        // We opened up an empty slot for the pivot,
                        //   only to find that it's a duplicate of the current item!
                        // Close the slot up again, and eliminate the duplicate.

                        for (i = low + 1; i < high; i++) {
                            distList[i] = distList[i + 1];
                            indexList[i] = indexList[i + 1];
                        }

                        // To eliminate the duplicate, overwrite its location with the
                        //   item from the end of the range, and then shrink the range
                        //   by one.

                        distList[high] = distList[rangeLast];
                        indexList[high] = indexList[rangeLast];
                        rangeLast--;

                        // The next iteration must not advance "high", since we've
                        //   just put a new element into it which needs to be processed.
                        // Decrementing it here will cancel out with the incrementation
                        //   of the next iteration.

                        high--;

                        // When we break the loop, the pivot element will be put
                        //   in its proper place ("low" + 1)
                        // Here, the proper place is where rangeLast used to be.
                        // To achieve this, we need to adjust "low" here.

                        low = rangeLast;

                        break;
                    }
                }
            }

            // If we've made it to here, we've found the insertion
            //   spot for the current element.
            // Perform the insertion.

            low++;
            distList[low] = pivotDist;
            indexList[low] = pivotIndex;

            // Move to the next item to be inserted in the growing sorted list.

            high++;
        }

        // Return the number of sorted items found.

        numFound = rangeLast - rangeFirst + 1;

        if (numFound > howMany) {
            numFound = howMany;
        }

        return numFound;
    }

    // The range to be sorted is large, so do a partial quicksort.
    // Select a pivot item, and swap it with the item at the beginning
    //   of the range.

    pivotLoc = rangeFirst + (rand.integer() % (rangeLast - rangeFirst + 1));
    tieBreakIndex = indexList[rand.integer() % (rangeLast - rangeFirst + 1)];

    pivotDist = distList[pivotLoc];
    distList[pivotLoc] = distList[rangeFirst];
    distList[rangeFirst] = pivotDist;

    pivotIndex = indexList[pivotLoc];
    indexList[pivotLoc] = indexList[rangeFirst];
    indexList[rangeFirst] = pivotIndex;

    // Eliminate all duplicates of the pivot.
    // Any duplicates found are pushed to the end of the range, and
    //   the range shrunk by one (thereby excluding them).

    i = rangeFirst + 1;

    while (i <= rangeLast) {
        if ((pivotIndex == indexList[i]) && (pivotDist == distList[i])) {
            distList[i] = distList[rangeLast];
            indexList[i] = indexList[rangeLast];
            rangeLast--;
        } else {
            i++;
        }
    }

    // Partition the remaining items with respect to the pivot.
    // This efficient method is adapted from the one outlined in
    //   Cormen, Leiserson & Rivest.
    // The range is scanned from both ends.
    // Items with small distances are placed below "low", and those
    //   with large distances are placed above "high".
    // Where "low" and "high" meet, the pivot item is inserted.

    low = rangeFirst;
    high = rangeLast + 1;

    while (TRUE) {
        // Move the "high" endpoint down until it meets either the pivot,
        //   or something that belongs on the "low" side.
        // If the key values are tied, decide pseudo-randomly.

        do {
            high--;
        } while (
                (distList[high] > pivotDist)
                ||
                (
                (distList[high] == pivotDist)
                &&
                (high > low)
                &&
                (
                (
                (tieBreakIndex >= pivotIndex)
                &&
                (
                (pivotIndex < indexList[high])
                &&
                (indexList[high] <= tieBreakIndex)
                )
                )
                ||
                (
                (tieBreakIndex < pivotIndex)
                &&
                (
                (pivotIndex < indexList[high])
                ||
                (indexList[high] <= tieBreakIndex)
                )
                )
                )
                )
                );

        // Move the "low" endpoint up until it meets either the "high" endpoint,
        //   or something that belongs on the "high" side.
        // If the key values are tied, decide pseudo-randomly.

        do {
            low++;
        } while (
                (low < high)
                &&
                (
                (distList[low] < pivotDist)
                ||
                (
                (distList[low] == pivotDist)
                &&
                (
                (
                (tieBreakIndex >= pivotIndex)
                &&
                (
                (indexList[low] <= pivotIndex)
                ||
                (tieBreakIndex < indexList[low])
                )
                )
                ||
                (
                (tieBreakIndex < pivotIndex)
                &&
                (
                (indexList[low] <= pivotIndex)
                &&
                (tieBreakIndex < indexList[low])
                )
                )
                )
                )
                )
                );

        // Have the "low" and "high" endpoints crossed?
        // If not, we still have more work to do.

        if (low < high) {
            // Swap the misplaced items, and try again.

            tempDist = distList[low];
            distList[low] = distList[high];
            distList[high] = tempDist;

            tempIndex = indexList[low];
            indexList[low] = indexList[high];
            indexList[high] = tempIndex;
        } else {
            // We found the cross-over point.

            break;
        }
    }

    // The pivot value ends up at the location referenced by "high".
    // Swap it with the pivot (which resides at the beginning of the range).

    distList[rangeFirst] = distList[high];
    distList[high] = pivotDist;

    indexList[rangeFirst] = indexList[high];
    indexList[high] = pivotIndex;

    pivotLoc = high;

    // The partition is complete.
    // Recursively sort the items with smaller distance.

    numFound = partialQuickSort
            (howMany, distList, indexList, rangeFirst, pivotLoc - 1);

    // If we found enough items (including the pivot), then we are done.
    // Make sure the pivot is in its correct position, if it is used.

    if (numFound >= howMany - 1) {
        if (numFound == howMany - 1) {
            distList[rangeFirst + numFound] = pivotDist;
            indexList[rangeFirst + numFound] = pivotIndex;
        }

        return howMany;
    }

    // We didn't find enough items, even taking the pivot into account.
    // Were any duplicates discovered during this call?

    if (numFound < pivotLoc - rangeFirst) {
        // Duplicates were discovered!
        // Figure out the minimum number of duplicates that must be
        //   replaced by items from the end of the range in order to
        //   leave the non-duplicates in contiguous locations.

        numDuplicatesToReplace = pivotLoc - rangeFirst - numFound;
        high = rangeLast;

        if (numDuplicatesToReplace > rangeLast - pivotLoc) {
            numDuplicatesToReplace = rangeLast - pivotLoc;
            rangeLast = rangeFirst + numFound + numDuplicatesToReplace;
        } else {
            rangeLast -= numDuplicatesToReplace;
        }

        // Replace the required number of duplicates by items from
        //   the end of the range.
        // The size of the range will shrink as a result.

        low = rangeFirst + numFound + 1;

        for (i = 0; i < numDuplicatesToReplace; i++) {
            distList[low] = distList[high];
            indexList[low] = indexList[high];
            low++;
            high--;
        }
    }

    // Put the pivot element in its proper place.

    distList[rangeFirst + numFound] = pivotDist;
    indexList[rangeFirst + numFound] = pivotIndex;

    // Finish up by sorting larger-distance items.
    // Note that the number of sorted items needed has dropped.

    return numFound + 1 + partialQuickSort
            (howMany - numFound - 1,
            distList, indexList,
            rangeFirst + numFound + 1, rangeLast);
}

/*!
 * Print statistics related to the RCT construction.
 *
 * @note Should only be called immediately after the construction!
 */
void RCT::printStats() const {
    cout << endl << "RCT build statistics:" << endl;
    cout << "  size                   == " << size << endl;
    cout << "  levels                 == " << levels << endl;
    cout << "  total nodes            == " << numNodes << endl;
    cout << "  max parents per node   == " << maxParents << endl;
    cout << "  max node degree        == " << maxDegree << endl;
    cout << "  avg node degree        == " << avgDegree << endl;
    cout << "  distance comparisons   == " << numDistComps << endl;
    cout << "  RNG seed               == " << seed << endl;
    cout << endl;
}

/*!
 * Sorts the items in the index and distance list in ascending order w.r.t.
 * their distances.
 *
 * @param distList List of item distances.
 * @param indexList List of item indices.
 * @param rangeFirst First element of range.
 * @param rangeLast Last element of range.
 */
void RCT::quickSort(float* distList, int* indexList, int rangeFirst, int rangeLast) {
    int pivotLoc = 0;
    float pivotDist;
    int pivotIndex;
    float tempDist;
    int tempIndex;
    int low = 0;
    int high = 0;
    float tieBreakDist;

    // If the range is empty, or if it contains only one item,
    //   then return immediately.

    if (rangeLast <= rangeFirst) {
        return;
    }

    // If the range to be sorted is small, just do an insertion sort.

    if (rangeLast - rangeFirst < 7) {
        high = rangeFirst + 1;
        tieBreakDist = distList[rand.integer() % (rangeLast - rangeFirst + 1)];

        // The outer while loop considers each item in turn (starting
        //   with the second item in the range), for insertion into
        //   the sorted list of items that precedes it.

        while (high <= rangeLast) {
            // Copy the next item to be inserted, as the "pivot".
            // Start the insertion tests with its immediate predecessor.

            pivotDist = distList[high];
            pivotIndex = indexList[high];
            low = high - 1;

            // Work our way down through previously-sorted items
            //   towards the start of the range.

            while (low >= rangeFirst) {
                // Compare the item to be inserted (the "pivot") with
                //   the current item.

                if (distList[low] < pivotDist) {
                    // The current item precedes the pivot in the sorted order.
                    // Break out of the loop - we have found the insertion point.

                    break;
                } else if (distList[low] > pivotDist) {
                    // The current item follows the pivot in the sorted order.
                    // Shift the current item one spot upwards, to make room
                    //   for inserting the pivot below it.

                    distList[low + 1] = distList[low];
                    indexList[low + 1] = indexList[low];
                    low--;
                } else {
                    // Break the tie pseudo-randomly.

                    if (
                            (
                            (tieBreakDist < pivotDist)
                            &&
                            (tieBreakDist < distList[low])
                            &&
                            (distList[low] < pivotDist)
                            )
                            ||
                            (
                            (tieBreakDist >= pivotDist)
                            &&
                            (
                            (tieBreakDist < distList[low])
                            ||
                            (distList[low] < pivotDist)
                            )
                            )
                            ) {
                        // The current item precedes the pivot in the sorted order.
                        // Break out of the loop - we have found the insertion point.

                        break;
                    } else {
                        // The current item follows the pivot in the sorted order.
                        // Shift the current item one spot upwards, to make room
                        //   for inserting the pivot below it.

                        distList[low + 1] = distList[low];
                        indexList[low + 1] = indexList[low];
                        low--;
                    }
                }
            }

            // If we've made it to here, we've found the insertion
            //   spot for the current element.
            // Perform the insertion.

            low++;
            distList[low] = pivotDist;
            indexList[low] = pivotIndex;

            // Move to the next item to be inserted in the growing sorted list.

            high++;
        }

        return;
    }

    // The range to be sorted is large, so do a quicksort.
    // Select a pivot item, and swap it with the item at the beginning
    //   of the range.

    pivotLoc = rangeFirst + (rand.integer() % (rangeLast - rangeFirst + 1));
    tieBreakDist = distList[rand.integer() % (rangeLast - rangeFirst + 1)];

    pivotDist = distList[pivotLoc];
    distList[pivotLoc] = distList[rangeFirst];
    distList[rangeFirst] = pivotDist;

    pivotIndex = indexList[pivotLoc];
    indexList[pivotLoc] = indexList[rangeFirst];
    indexList[rangeFirst] = pivotIndex;

    // Partition the items with respect to the pivot.
    // This efficient method is adapted from the one outlined in
    //   Cormen, Leiserson & Rivest.
    // The range is scanned from both ends.
    // Items with small distances are placed below "low", and those
    //   with large distances are placed above "high".
    // Where "low" and "high" meet, the pivot item is inserted.

    low = rangeFirst;
    high = rangeLast + 1;

    while (TRUE) {
        // Move the "high" endpoint down until it meets either the pivot,
        //   or something that belongs on the "low" side.
        // If the key values are tied, decide pseudo-randomly.

        do {
            high--;
        } while (
                (distList[high] > pivotDist)
                ||
                (
                (distList[high] == pivotDist)
                &&
                (
                (
                (tieBreakDist >= pivotDist)
                &&
                (pivotDist < distList[high])
                &&
                (distList[high] <= tieBreakDist)
                )
                ||
                (
                (tieBreakDist < pivotDist)
                &&
                (
                (pivotDist < distList[high])
                ||
                (distList[high] <= tieBreakDist)
                )
                )
                )
                )
                );

        // Move the "low" endpoint up until it meets either the pivot,
        //   or something that belongs on the "high" side.
        // If the key values are tied, decide pseudo-randomly.

        do {
            low++;
        } while (
                (low < high)
                &&
                (
                (distList[low] < pivotDist)
                ||
                (
                (distList[low] == pivotDist)
                &&
                (
                (
                (tieBreakDist < pivotDist)
                &&
                (tieBreakDist < distList[low])
                &&
                (distList[low] < pivotDist)
                )
                ||
                (
                (tieBreakDist >= pivotDist)
                &&
                (
                (tieBreakDist < distList[low])
                ||
                (distList[low] < pivotDist)
                )
                )
                )
                )
                )
                );

        // Have the "low" and "high" endpoints crossed?
        // If not, we still have more work to do.

        if (low < high) {
            // Swap the misplaced items, and try again.

            tempDist = distList[low];
            distList[low] = distList[high];
            distList[high] = tempDist;

            tempIndex = indexList[low];
            indexList[low] = indexList[high];
            indexList[high] = tempIndex;
        } else {
            // We found the cross-over point.

            break;
        }
    }

    // The pivot value ends up at the location referenced by "high".
    // Swap it with the pivot (which resides at the beginning of the range).

    distList[rangeFirst] = distList[high];
    distList[high] = pivotDist;

    indexList[rangeFirst] = indexList[high];
    indexList[high] = pivotIndex;

    // The partition is complete.
    // Recursively sort the remaining items.

    quickSort(distList, indexList, rangeFirst, high - 1);
    quickSort(distList, indexList, high + 1, rangeLast);
}

/*!
 * Reserve storage for the RCT and items data.
 */
void RCT::reserveStorage() {
    int i;
    int lvl;

    // Reserve storage for the mapping between internal and external
    //   data indices.

    internToExternMapping = new int [size];

    for (i = 0; i < size; i++) {
        internToExternMapping[i] = i;
    }

    // Set up storage for child-to-parent edges and parent-to-child edges.

    parentIndexLLList = new int** [levels + 1];
    parentLSizeLList = new int* [levels + 1];

    childIndexLLList = new int** [levels + 1];
    childLSizeLList = new int* [levels + 1];

    for (lvl = 0; lvl <= levels; lvl++) {
        parentIndexLLList[lvl] = new int* [levelSetSizeList[lvl]];
        parentLSizeLList[lvl] = new int [levelSetSizeList[lvl]];

        childIndexLLList[lvl] = new int* [levelSetSizeList[lvl]];
        childLSizeLList[lvl] = new int [levelSetSizeList[lvl]];

        for (i = levelSetSizeList[lvl] - 1; i >= 0; i--) {
            parentIndexLLList[lvl][i] = NULL;
            parentLSizeLList[lvl][i] = 0;

            childIndexLLList[lvl][i] = NULL;
            childLSizeLList[lvl][i] = 0;
        }
    }

    // Set up storage for managing distance computations and
    //   query results.

    distFromQueryList = new float [size];
    storedDistIndexList = new int [size];
    numStoredDists = 0;

    queryResultDistList = new float [size];
    queryResultIndexList = new int [size];
    queryResultSize = 0;
    queryResultSampleSize = 0;

    visitedNodeIndexList = new int [size];
    tempResultIndexList = new int [size];
    tempResultDistList = new float [size];

    for (i = 0; i < size; i++) {
        distFromQueryList[i] = RCT_UNKNOWN_;
        storedDistIndexList[i] = RCT_NONE_;

        queryResultDistList[i] = RCT_UNKNOWN_;
        queryResultIndexList[i] = RCT_NONE_;

        visitedNodeIndexList[i] = FALSE;

        tempResultIndexList[i] = RCT_NONE_;
        tempResultDistList[i] = RCT_UNKNOWN_;
    }
}

/*!
 * Accepts a new item as the query object for future distance comparisons.
 * Any previously-stored distances are cleared by this operation, except in the
 * case where the previous query object is identical to the current query
 * object.
 *
 * @param query The new query item.
 */
void RCT::setNewQuery(DistData* query) {
    int i;
    if (query != this->query) {
        for (i = 0; i < numStoredDists; i++) {
            distFromQueryList[storedDistIndexList[i]] = RCT_UNKNOWN_;
        }
        this->query = query;
        numStoredDists = 0;
    }
}

/*!
 * Reserve storage for the level set information. The number of RCT items and
 * maximum number of node parents must be given.
 *
 * @note This operation determines the shape of the RCT without actually
 *       assigning any items to it.
 */
void RCT::setupLevels(int numItems, int numParents) {
    int i;
    int lvl;
    float coin = 0.0f;
    int* maxLevelList = NULL;

    size = numItems;

    if (numParents <= 1) {
        maxParents = 1;
    } else {
        maxParents = numParents;
    }

    // Determine the number of RCT levels, and
    //   which level sets each member belongs to.
    if (size > 1) {
        maxLevelList = new int [size];
        levels = 1;
        float p = 1.0f / sampleRate;

        for (i = 0; i < size; ++i) {
            maxLevelList[i] = 0;
            while (rand() <= p) {
                ++maxLevelList[i];
            }
            if (maxLevelList[i] >= levels) {
                levels = 1 + maxLevelList[i];
            }
        }
    } else {
        levels = 0;
    }

    // Determine the level sample sizes.

    levelQuotaList = new int [levels + 1];
    levelSetSizeList = new int [levels + 1];

    for (lvl = 0; lvl < levels; lvl++) {
        levelQuotaList[lvl] = 0;
        levelSetSizeList[lvl] = 0;
    }

    levelQuotaList[levels] = 0;
    levelSetSizeList[levels] = 1;

    for (i = 0; i < size; i++) {
        for (lvl = maxLevelList[i]; lvl >= 0; lvl--) {
            levelSetSizeList[lvl]++;
        }
    }

    numNodes = 0;

    for (lvl = 0; lvl <= levels; lvl++) {
        numNodes += levelSetSizeList[lvl];
    }

    // Delete the max level list, since we don't need it any more.

    if (maxLevelList != NULL) {
        delete [] maxLevelList;
        maxLevelList = NULL;
    }
}

/*!
 * Get the fraction of edges in the RCT that are well-formed. An edge from a
 * parent <em>p</em> on level <em>i</em> to a child <em>c</em> is consider
 * well-formed if and only if there is no item <em>p'</em> on level <em>i</em>,
 * such that <em>dist(p',c)</em> is strictly less than <em>dist(p,c)</em>.
 * 
 * @note The RCT is well-formed if and only if all of its edges are well-formed.
 *       Edges between the artificial root node and its children are by
 *       definition always well-formed and are not checked.
 *
 * @note Be aware that checking well-formedness is very expensive due to the
 *       required exact nearest-neighbor queries!
 *
 * @return Fraction of edges that are well-formed.
 */
double RCT::getFractionOfWellformedEdges() {
    // Count the well-formed edges.
    long long wellFormedEdges = 0LL;
    long long edgesChecked = 0LL;

    // Check all levels:
    for (int L = levels - 1; L > 0; --L) {
        // Check all nodes on level L.
        for (int i = 0; i < levelSetSizeList[L]; ++i) {
            // Select an item on level L as the parent.
            DistData* parent = data[internToExternMapping[i]];

            // Investigate each child of that parent.
            int numChildren = childLSizeLList[L][i];
            int* children = childIndexLLList[L][i];
            for (int j = 0; j < numChildren; ++j) {
                // Retrieve the child item.
                DistData* child = data[internToExternMapping[children[j]]];

                // Is this a copy of the parent?
                if (parent == child) {
                    ++wellFormedEdges;
                    ++edgesChecked;
                    continue;
                }

                // Determine the actual parent-child distance.
                float actualParentChildDistance = parent->distanceTo(child);

                // Find the nearest-neighbor's distance of the child at the parent level L.
                setNewQuery(child);
                doFindNearest(1, L);
                float correctParentChildDistance;
                getResultDists(&correctParentChildDistance, 1);

                // Do the distances match?
                if (actualParentChildDistance == correctParentChildDistance) {
                    ++wellFormedEdges;
                }
                ++edgesChecked;
            }
        }
    }

    // How many edges do we have in the RCT?
    cout << wellFormedEdges << "/" << edgesChecked << endl;
    return static_cast<double>(wellFormedEdges) / static_cast<double>(edgesChecked);
}
