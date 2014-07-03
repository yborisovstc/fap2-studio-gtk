
#ifndef __FAP2STU_COMMON_H
#define __FAP2STU_COMMON_H

#include <gtkmm/main.h>

// Parameters of view
const gint KViewCompAreaWidth = 200;
// Size of vertical tunnel between component representation - minimum
const gint KViewCompGapHight = 20;
// Size of vertical tunnel between component representation - maximum
const gint KViewCompGapHightMax = 100;
const gint KViewConnGapWidth = 3;
const gint KViewConnGapHeight = 1;
const gint KViewCompEmptyBodyHight = 20;
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
// TODO not used, consider to removec
const gint KEdgePointWidth = 9;
// Edges border size
const gint KEdgeBorderWidth = 4;
// Edges grid cell size
const gint KEdgeGridCell = 10;
// Minimal width of CRPs body
const gint KCrpBodyMinWidth = 10;
// DRP Internal padding
const gint KDrpPadding = 8;
// Gap between vertical layout area and vertical edges tunnel
const gint KEvtGap = 2*KEdgeGridCell;

// Targets entries infos for drag-drop
const gint KTei_EdgeCp = 4;
const gint KTei_ElemCrp = 5;
// Distance Threshold when DnD edge to connectable component
const gint KDistThresholdEdge = 90;

// DnD targets type
enum TDnDTarg {
    EDT_Unknown = 0,
    EDT_Node = 1,
    EDT_EdgeCp = 2,
    EDT_Comp = 3, // Local component
};

// TODO [YB] To rename to FAP_NODE_URI
static const gchar* KDnDTarg_Comp = "FAP_COMP";
static const gchar* KDnDTarg_EdgeCp = "FAP_EDGE_CP";

#endif
