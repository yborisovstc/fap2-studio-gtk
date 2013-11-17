
#ifndef __FAP2STU_COMMON_H
#define __FAP2STU_COMMON_H

#include <gtkmm/main.h>

// Parameters of view
const gint KViewNameLineHeight = 20;
const gint KViewExtLineHeight = 20;
const gint KViewExtAreaWidth = 90;
const gint KViewCompAreaWidth = 200;
const gint KViewCompGapHight = 20;
const gint KViewStateWidth = 300;
const gint KViewConnLineLen = 10;
const gint KViewConnLabelGapWidth = 2;
const gint KViewConnIdHeight = 16;
const gint KViewConnIdWidth = 120;
const gint KViewConnIdMaxNum = 1;
const gint KViewConnGapWidth = 3;
const gint KViewConnGapHeight = 1;
const gint KViewTransWidth = 200;
const gint KViewTransHeight = 100;
const gint KViewStateGapTransInp = 20;
const gint KViewCompInpOutpGapWidth = 20;
const gint KViewCompCpGapHeight = 3;
const gint KViewCompEmptyBodyHight = 20;
const gint KViewDectGapWidth = 0;
const gint KViewExtCompGapWidth = 40;
const gint KViewStateTransBorder = 1;
const gint KViewStateTransIntBorder = 2;
const gint KViewBtnHeightFromHintVar = 4;
const gint KViewBtnBoxInnerBoard = 0;
const gint KViewTransMinWidth = 30;
const gint KViewBorder = 2;
const gint KCompHeaderLabelsGap = 8;
const gint KConnHorizGap = 8;
const gint KConnVertGap = 10;
const gint KConnHorizSpreadMin = 20;
const gint KViewElemCrpInnerBorder = 8;
const gint KViewElemContLinesVGap = 4;
// Width of gap betweed boundary comp and view border
const gint KBoundCompGapWidth = 4;
// Edge's points width and height
// TODO not used, consider to removec:w
const gint KEdgePointWidth = 9;
// Edges border size
const gint KEdgeBorderWidth = 4;
// Edges grid cell size
const gint KEdgeGridCell = 10;

// Targets entries infos for drag-drop
const gint KTei_EdgeCp = 4;
const gint KTei_ElemCrp = 5;
// Distance Threshold when DnD edge to connectable component
const gint KDistThresholdEdge = 90;

// DnD targets type
enum TDnDTarg {
    EDT_Unknown = 0,
    EDT_AddingNode = 1,
    EDT_EdgeCp = 2,
    EDT_Comp = 3, // Local component
};

// TODO [YB] To rename to FAP_NODE_URI
static const gchar* KDnDTarg_Comp = "FAP_COMP";

#endif
