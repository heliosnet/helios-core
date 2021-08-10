//
//  CVSimpleCVQueue.h
//  CVNetwork
//
//  Created by Filipi Nascimento Silva on 11/23/21.
//  Copyright (c) 2021 Filipi Nascimento Silva. All rights reserved.
//

#ifndef CVNetwork_CVPartition_h
#define CVNetwork_CVPartition_h
#include "CVCommons.h"
#include "CVSet.h"
#include "CVDictionary.h"

typedef struct CVPartition {
	CVNetwork* network;
	CVSize maxGroupCount;
	CVUIntegerSetRef* groups;
	CVUInteger* membership;
	CVUIntegerSetRef availableGroups;
} CVPartition;

typedef enum CVPartitionSAStepType{
	CVPartitionSAStepTypeNone = 0,
	CVPartitionSAStepTypeFailed = 1,
	CVPartitionSAStepTypeRefused = 2,
	CVPartitionSAStepTypeMove = 3,
	CVPartitionSAStepTypeMerge = 4,
	CVPartitionSAStepTypeSplit = 5
} CVPartitionSAStepType;
static const CVString kCVPartitionSAStepTypeText = "0-_VGS";
static const CVString kCVPartitionSAStepTypeTextLower = "0-_vgs";

typedef CVPartition* CVPartitionRef;

CVPartitionRef CVNewPartition(CVNetwork* network, CVSize maxGroupCount);
void CVPartitionRandomInit(CVPartitionRef partition, CVSize groupCount);

CVPartitionRef CVNewPartitionFromMembership(CVNetwork* network, CVUInteger *membership, CVSize maxGroupCount);
CVPartitionRef CVPartitionCopy(CVPartitionRef partition);


void CVPartitionDestroy(CVPartitionRef partition);


typedef struct CVPartitionSets {
	CVPointerSetRef groups;
	CVUInteger mincut;
} CVPartitionSets;

typedef CVPartitionSets* CVPartitionSetsRef;


CVPartitionSetsRef CVNewPartitionSets();

CVPartitionSetsRef CVNewPartitionSetsKargerMinimumCut(
	CVEdge *edges,
	CVSize edgeCount
	);

void CVPartitionSetsDestroy(CVPartitionSetsRef groups) ;

CVPartitionSetsRef CVNewPartitionSetsBestKargerMinimumCut(
	CVEdge *edges,
	CVSize edgeCount,
	CVSize iterations
	);




CVBool CVPartitionMoveVertexToGroup(
	CVPartitionRef partition,
	CVUInteger vertexIndex,
	CVUInteger groupIndex);

CVBool CVPartitionSplitGroup(
	CVPartitionRef partition,
	CVUInteger groupIndex,
	CVSize mincutIterations);

CVBool CVPartitionMergeGroups(
	CVPartitionRef partition,
	CVUInteger groupIndex1,
	CVUInteger groupIndex2);

CVDouble CVPartitionCalculateModularity(CVPartitionRef partition);

CVDouble CVPartitionCalculateModularityConstrained(
	CVPartitionRef partition,
	CVSize minimumSize,
	CVSize maximumSize,
	CVDouble beta);


void CVPartitionPrintMemberships(CVPartitionRef partition);
void CVPartitionPrintGroups(CVPartitionRef partition);



CVPartitionRef CVNewPartitionFromSAStep(
	CVPartitionRef partition,
	CVDouble splitProbability,
	CVDouble mergeProbability,
	CVSize mincutIterations,
	CVPartitionSAStepType* returnStepType);


CVBool _CVPartitionSplitRandomGroup(
	CVPartitionRef partition,
	CVSize mincutIterations
	);	
#endif