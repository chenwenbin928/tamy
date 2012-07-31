/// @file   TamyEditor/ProfilerFrame.h
/// @brief  a frame that shows the engine profiler results
#pragma once

#include "ResourceEditor.h"
#include "core/Profiler.h"
#include <vector>


///////////////////////////////////////////////////////////////////////////////

class QTreeWidget;
class QTreeWidgetItem;
class QFormLayout;
class ProfilerChart;
class QLabel;

///////////////////////////////////////////////////////////////////////////////

/**
 * A frame that shows the engine profiler results.
 */
class ProfilerFrame : public ResourceEditor
{
   Q_OBJECT

private:
   enum CallstackTreeColumns
   {
      CALLSTACK_COL_TIMER_ID,
      CALLSTACK_COL_NAME,
   };

   struct ChartDesc
   {
      ProfilerChart*    m_chart;
      QLabel*           m_topValueLabel;

      ChartDesc( ProfilerChart* chart, QLabel* topValueLabel ) : m_chart( chart ), m_topValueLabel( topValueLabel ) {}
   };

private:
   QTreeWidget*                     m_callstackTree;
   QFormLayout*                     m_chartsLayout;

   std::vector< ChartDesc >         m_charts;

public:
   /**
    * Constructor.
    */
   ProfilerFrame();

   /**
    * Call this method to update profiling information.
    */
   void update( float timeElapsed );

   // -------------------------------------------------------------------------
   // ResourceEditor implementation
   // -------------------------------------------------------------------------
   void onInitialize();
   void onDeinitialize( bool saveProgress );

public slots:
   void showSelectedTimerProfile( QTreeWidgetItem* timerItem, int clickedColumnIdx );

private:
   // callstack tree related methods
   void findCallstackItem( const Profiler::Trace* prevTrace, const Profiler::Trace& trace, QTreeWidgetItem*& parentItem, QTreeWidgetItem*& item ) const;
   QTreeWidgetItem* addCallstackItem( QTreeWidgetItem* parentItem, const Profiler& profiler, const Profiler::Trace& trace );

   /**
    * A helper method that extracts a timer id from the specified tree item.
    */
   uint getTimerId( QTreeWidgetItem* item ) const;
};

///////////////////////////////////////////////////////////////////////////////
