#pragma once

/* VTK Library files for 2D plotting*/
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderWindow.h>
#include <vtkSmartPointer.h>
#include <vtkChartXY.h>
#include <vtkAxis.h>
#include <vtkTable.h>
#include <vtkPlot.h>
#include <vtkPlotPoints.h>
#include <vtkFloatArray.h>
#include <vtkContextView.h>
#include <vtkContextScene.h>
#include <vtkPen.h>
#include <vtkNamedColors.h>

/* External modules */
#include <vector>
#include <string>
#include <iostream>


// Namespace "wrapped visualization toolkit" 
namespace W_VTK {

	namespace _2D {

		/* Enum for code clarity */
		enum {
			X_axis
		};

		/* =-=-=-=-=-=-=-=-=-=-=-==-=-=-=-=-=-=-=-=-=-=-==-=-=-=-=-=-=-=-=-=-=-==-=-=-=-=-=-=-=-=-=-=-==-=-=-=-=-=-=-=-=-=-=-= */
		/* ------------------- 2D Line plotters: Stack memory variant (memory must be known at compile time) ----------------- */
		/* =-=-=-=-=-=-=-=-=-=-=-==-=-=-=-=-=-=-=-=-=-=-==-=-=-=-=-=-=-=-=-=-=-==-=-=-=-=-=-=-=-=-=-=-==-=-=-=-=-=-=-=-=-=-=-= */

		/*=================================================================================================================
		Plot N number of lines with data points sharing the same X coordinates for each data set.
		=================================================================================================================== */
		template<int numPoints, int numLines> 		// Line properties are random colours and width of 1.0 
		void Line_plotter(std::string(&names)[numLines], float(&x_pos)[numPoints], float(&data)[numLines][numPoints]) {

			/* ----- Notes -----:
			Input data		-> rows = Line number, cols = points on lines
			vtKtable data	-> rows = points on lines, cols = Line number
			YOU HAVE TO NAME YOUR LINES AND YOU HAVE TO NAME THEM DIFFERENTLY
			*/

			// Create a table with some points in it
			vtkSmartPointer<vtkTable> table = vtkSmartPointer<vtkTable>::New();

			// Instantiate an array of vtkFloatArray type. (Holds line data )
			vtkSmartPointer<vtkFloatArray> arr[numLines + 1];

			// Instantiate the x-axis (Cant use condition in allocation loop for some reason...)
			arr[X_axis] = vtkSmartPointer<vtkFloatArray>::New();
			arr[X_axis]->SetName("X-axis");
			table->AddColumn(arr[X_axis]);

			// Allocate float array data and give names. Note numLines + 1 as you have to include x-axis. 
			for (int j = 1; j < numLines + 1; j++) {

				// Allocate float array data
				arr[j] = vtkSmartPointer<vtkFloatArray>::New();

				// NAMING LINES MATTERS BECAUSE IF YOU DONT VTK WONT WORK. THEY ALSO NEED DIFFERENT NAMES.
				arr[j]->SetName(names[j - 1].c_str());	// j-1 to offset data (as x-axis was added)

				// Add collumn to vtkTable with vtkFloatarray memory 
				table->AddColumn(arr[j]);
			}

			// Set number of rows on table for number of points 
			table->SetNumberOfRows(numPoints);

			// Set/transform raw data to vtkTable 
			for (int i = 0; i < numLines; i++) {
				for (int j = 0; j < numPoints; j++) {
					table->SetValue(j, 0, x_pos[j]);		// Recall x axis is vtkTable col [0].
					table->SetValue(j, i + 1, data[i][j]);	// Access data col-wise for efficiency (col0,col1,col2,col3 etc..) 
															// i+1 to put data on correct collumns on vtkTable
				}
			}

			// Set up the view
			vtkSmartPointer<vtkContextView> view = vtkSmartPointer<vtkContextView>::New();
			view->GetRenderer()->SetBackground(1.0, 1.0, 1.0);

			// Add multiple line plots, setting the colors etc
			vtkSmartPointer<vtkChartXY> chart = vtkSmartPointer<vtkChartXY>::New();
			view->GetScene()->AddItem(chart);

			// Instantiate a vtkLine object 
			vtkPlot* line = chart->AddPlot(vtkChart::LINE);

			// RGB array for random colours for plotting multiple lines 
			double RGB[3]; 

			// Loop over lines and input data to vtkLine for plotting: Line -> chart -> view -> Renderwindow, Interactor
			for (int j = 1; j < numLines + 1; j++)
			{
				// Add a plot for each line after first iteration. 
				if (j > 1) {
					line = chart->AddPlot(vtkChart::LINE);
				}
				line->SetInputData(table, 0, j);
				
				// Line properties
				RGB[0] = std::rand() % 255;
				RGB[1] = std::rand() % 255;
				RGB[2] = std::rand() % 255;
				line->SetColor(RGB[0],RGB[1],RGB[2]);
				line->SetWidth(1.0);
			}

			// Start interactor and render window
			view->GetRenderWindow()->Render();
			view->GetInteractor()->Initialize();
			view->GetInteractor()->Start();
		}


		/* =================================================================================================================
		Plot N number of lines where data points have different X coordinates for each data set.
		Note: Good for automating plotting for alot of lines from big data. (i.e. if you want to plot 100 lines from data)
		=================================================================================================================  */

		/* Function to generate generic line names for any amount of lines in a plot.
		   - VTK requires lines to be named and to be different names!					*/
		template<int no_of_names> 
		void LineNameGenerator(std::string(&names)[no_of_names]) {

			for (int i = 0; i < no_of_names; i++) {
				names[i] = "Line: " + std::to_string(i);
			}
		}

		template<int numPoints, int numLines>			// Line properties are random colours and width of 1.0 
		void Line_plotter(std::string(&names)[2 * numLines], float(&x_pos)[numLines][numPoints], float(&data)[numLines][numPoints]) {

			/* ----- Notes -----:
			Input data		-> rows = Line number, cols = points on lines
			vtKtable data	-> rows = points on lines, cols = Line number
			YOU HAVE TO NAME YOUR LINES AND YOU HAVE TO NAME THEM DIFFERENTLY */

			// Create a tablefor array data
			vtkSmartPointer<vtkTable> table = vtkSmartPointer<vtkTable>::New();

			// Instantiate an array of vtkFloatArray type. (Holds line data )
			vtkSmartPointer<vtkFloatArray> arr[2 * numLines];

			// Allocate float array data and give names. Note numLines + 1 as you have to include x-axis. 
			for (int j = 0; j < 2 * numLines; j++) {

				// Allocate float array data
				arr[j] = vtkSmartPointer<vtkFloatArray>::New();

				// NAMING LINES MATTERS BECAUSE IF YOU DONT VTK WONT WORK. THEY ALSO NEED DIFFERENT NAMES. 
				arr[j]->SetName(names[j].c_str());

				// Add collumn to vtkTable with vtkFloatarray memory 
				table->AddColumn(arr[j]);
			}

			// Set number of rows on table for number of points 
			table->SetNumberOfRows(numPoints);

			// Set/transform raw data to vtkTable (rows -> pts, cols -> Lines : The way VTK data works..) 
			for (int i = 0; i < numLines; i++) {
				for (int j = 0; j < numPoints; j++) {

					table->SetValue(j, i, x_pos[i][j]);
					table->SetValue(j, i + numLines, data[i][j]);	// Accss data col-wise for efficiency (col0,col1,col2,col3 etc..) 
				}
			}

			// Set up the view
			vtkSmartPointer<vtkContextView> view = vtkSmartPointer<vtkContextView>::New();
			view->GetRenderer()->SetBackground(1.0, 1.0, 1.0);

			// Add multiple line plots, setting the colors etc
			vtkSmartPointer<vtkChartXY> chart = vtkSmartPointer<vtkChartXY>::New();
			view->GetScene()->AddItem(chart);

			// Instantiate a vtkLine object 
			vtkPlot* line = chart->AddPlot(vtkChart::LINE);

			// RGB array for line colour data 
			double RGB[3];

			// Loop over lines and input data to vtkLine for plotting: Line -> chart -> view -> Renderwindow, Interactor
			for (int j = 0; j < numLines; j++) {

				// Add a plot for each line after first iteration. 
				if (j > 0) {
					line = chart->AddPlot(vtkChart::LINE);
				}
				line->SetInputData(table, j, j + numLines);

				// Line properties
				RGB[0] = std::rand() % 255;
				RGB[1] = std::rand() % 255;
				RGB[2] = std::rand() % 255;
				line->SetColor(RGB[0], RGB[1], RGB[2]);
				line->SetWidth(1.0);
			}

			// Start interactor
			view->GetRenderWindow()->Render();
			view->GetInteractor()->Initialize();
			view->GetInteractor()->Start();
		}


		/* =================================================================================================================
		Plotting single lines on the same render window (Lines not together in memory (e.g. data_set_1[10], data_set_2[10]))
		Note: Works with different X coordinates for each data set.
		================================================================================================================= */

		// Function to instatiate a vtkChart data structure for containing line plot data
		/* From documentation:	"In this case the smart pointer automatically manages the reference it owns. When the smart pointer variable goes out-of-scope
								and is no longer used, such as when a function in which it is a local variable returns, it automatically informs the object by
								decrementing the reference count. By using the static New() method provided by the smart pointer no raw pointer ever needs to
								hold a reference to the object, so no call to Delete() is needed."	*/
		vtkSmartPointer<vtkChartXY> multiplot_chart_instantiation() {

			// Set up chart data structure
			vtkSmartPointer<vtkChartXY> chart = vtkSmartPointer<vtkChartXY>::New();

			return chart;
		}

		// Function to start the render window and interactor. 
		void multiplot_view_window(vtkSmartPointer<vtkChartXY>& chart, const char* BackgroundColour, bool showLegend) {

			// vtkNamedColors:  A class holding colors and their names.
			// For info see the folder with VTK_Coloursheets for different colour names. 
			vtkSmartPointer<vtkNamedColors> colors = vtkSmartPointer<vtkNamedColors>::New();

			// Set up the view
			vtkSmartPointer<vtkContextView> view = vtkSmartPointer<vtkContextView>::New();
			view->GetRenderer()->SetBackground(colors->GetColor3d(BackgroundColour).GetData());	// Set the background to white
			
			// Show legend?
			chart->SetShowLegend(showLegend);


			// Add the chart to the view
			view->GetScene()->AddItem(chart);

			// Start interactor
			view->GetRenderWindow()->Render();
			view->GetInteractor()->Initialize();
			view->GetInteractor()->Start();
		}


		// Single 2D Line creator which adds the plot of a 2D line to the inputted view
		template<int numPoints>
		void Line_plotter(vtkSmartPointer<vtkChartXY>& chart, float(&x_pos)[numPoints], float(&data)[numPoints], std::string& name, const char* LineColour, float width) {

			/* ----- Notes -----:
			Input data		-> rows = Line number, cols = points on lines
			vtKtable data	-> rows = points on lines, cols = Line number
			YOU HAVE TO NAME YOUR LINES AND YOU HAVE TO NAME THEM DIFFERENTLY
			*/

			// Create a table with some points in it
			vtkSmartPointer<vtkTable> table = vtkSmartPointer<vtkTable>::New();

			// Instantiate an array of vtkFloatArray type. (Holds line data )
			vtkSmartPointer<vtkFloatArray> arr[2];

			// Instantiate the x-axis (Cant use condition in allocation loop for some reason...)
			arr[X_axis] = vtkSmartPointer<vtkFloatArray>::New();
			arr[X_axis]->SetName("X-axis");
			table->AddColumn(arr[X_axis]);

			// Instantiate data for next line: 
			// Allocate float array data and give name
			arr[1] = vtkSmartPointer<vtkFloatArray>::New();			// Allocate float array data
			arr[1]->SetName(name.c_str());									// Set name
			table->AddColumn(arr[1]);								// Add collumn to vtkTable with vtkFloatarray memory 

			// Set number of rows on table for number of points 
			table->SetNumberOfRows(numPoints);

			// Set/transform raw data to vtkTable 

			for (int i = 0; i < numPoints; i++) {
				table->SetValue(i, 0, x_pos[i]);		// Recall x axis is vtkTable col [0].
				table->SetValue(i, 1, data[i]);
			}


			// vtkNamedColors:  A class holding colors and their names.
			// For info see the folder with VTK_Coloursheets for different colour names. 
			vtkSmartPointer<vtkNamedColors> colors = vtkSmartPointer<vtkNamedColors>::New();

			// Instantiate a vtkLine object 
			vtkPlot* line = chart->AddPlot(vtkChart::LINE);
			line->SetInputData(table, 0, 1);
			line->GetPen()->SetColorF(colors->GetColor3d(LineColour).GetData());
			// line->SetColor(0, 255, 0, 255);
			line->SetWidth(width);
		}


		/* =================================================================================================================
		Plotting scatter plots (points) which share the same X coordinate in each dataset 
		================================================================================================================= */
		template<int numPoints, int numDataSets>  // Line properties are random colours and width of 1.0 
		void Scatter_plotter(std::string(&names)[numDataSets], float(&x_pos)[numPoints], float(&data)[numDataSets][numPoints]) {

			/* ----- Notes -----:
			Input data		-> rows = dataset number, cols = points on dataset
			vtKtable data	-> rows = points on dataset, cols = dataset
			YOU HAVE TO NAME YOUR DATASETS AND YOU HAVE TO NAME THEM DIFFERENTLY
			*/

			// Create a table with some points in it
			vtkSmartPointer<vtkTable> table = vtkSmartPointer<vtkTable>::New();

			// Instantiate an array of vtkFloatArray type. (Holds line data )
			vtkSmartPointer<vtkFloatArray> arr[numDataSets + 1];

			// Instantiate the x-axis (Cant use condition in allocation loop for some reason...)
			arr[X_axis] = vtkSmartPointer<vtkFloatArray>::New();
			arr[X_axis]->SetName("X-axis");
			table->AddColumn(arr[X_axis]);

			// Allocate float array data and give names. Note numLines + 1 as you have to include x-axis. 
			for (int j = 1; j < numDataSets + 1; j++) {

				// Allocate float array data
				arr[j] = vtkSmartPointer<vtkFloatArray>::New();

				// NAMING LINES MATTERS BECAUSE IF YOU DONT VTK WONT WORK. THEY ALSO NEED DIFFERENT NAMES.
				arr[j]->SetName(names[j - 1].c_str());	// j-1 to offset data (as x-axis was added)

				// Add collumn to vtkTable with vtkFloatarray memory 
				table->AddColumn(arr[j]);
			}

			// Set number of rows on table for number of points 
			table->SetNumberOfRows(numPoints);

			// Set/transform raw data to vtkTable 
			for (int i = 0; i < numDataSets; i++) {
				for (int j = 0; j < numPoints; j++) {
					table->SetValue(j, 0, x_pos[j]);		// Recall x axis is vtkTable col [0].
					table->SetValue(j, i + 1, data[i][j]);	// Access data col-wise for efficiency (col0,col1,col2,col3 etc..) 
															// i+1 to put data on correct collumns on vtkTable
				}
			}

			// Set up the view
			vtkSmartPointer<vtkContextView> view = vtkSmartPointer<vtkContextView>::New();
			view->GetRenderer()->SetBackground(1.0, 1.0, 1.0);

			// Add multiple line plots, setting the colors etc
			vtkSmartPointer<vtkChartXY> chart = vtkSmartPointer<vtkChartXY>::New();
			view->GetScene()->AddItem(chart);

			// RGB array for random colours for plotting multiple lines 
			double RGB[3];

			// Instantiate a vtkPlot object with POINTS
			vtkPlot* points = chart->AddPlot(vtkChart::POINTS);

			// Loop over lines and input data to vtkLine for plotting: Line -> chart -> view -> Renderwindow, Interactor
			for (int j = 1; j < numDataSets + 1; j++) {

				// Add a plot for each line after first iteration. 
				if (j > 1) {
					points = chart->AddPlot(vtkChart::POINTS);
				}
				points->SetInputData(table, 0, j);

				// Line properties
				RGB[0] = std::rand() % 255;
				RGB[1] = std::rand() % 255;
				RGB[2] = std::rand() % 255;
				points->SetColor(RGB[0], RGB[1], RGB[2]);
				points->SetWidth(1.0);
				dynamic_cast<vtkPlotPoints*>(points)->SetMarkerStyle(vtkPlotPoints::CIRCLE); // CIRCLE = 4 in shape enum in vtkPlotPoints class

				// Dynamic_cast for runtime checks on polymorphism 
				// Polymorphism used as chart does not take vtkPlotPoints as an input (I think)
				// e.g. setMarkerStyle not available in vtkPlot but is in vtkPlotPoints. 
			}

			// Start interactor and render window
			view->GetRenderWindow()->Render();
			view->GetInteractor()->Initialize();
			view->GetInteractor()->Start();
		
		}



		/* =================================================================================================================
		Plot N number of scatter datasets where data points have different X coordinates for each data set.
		Note: Good for automating plotting for alot of datasets from big data. (i.e. if you want to plot 100 scatter sets from data)
		=================================================================================================================  */

		/* Function to generate generic line names for any amount of lines in a plot.
		   - VTK requires lines to be named and to be different names!					*/
		template<int no_of_names>
		void ScatterNameGenerator(std::string(&names)[no_of_names]) {

			for (int i = 0; i < no_of_names; i++) {
				names[i] = "Scatter dataset: " + std::to_string(i);
			}
		}

		template<int numPoints, int numDataSets>			// Line properties are random colours and width of 1.0 
		void Scatter_plotter(std::string(&names)[2 * numDataSets], float(&x_pos)[numDataSets][numPoints], float(&data)[numDataSets][numPoints]) {

			/* ----- Notes -----:
			Input data		-> rows = Line number, cols = points on lines
			vtKtable data	-> rows = points on lines, cols = Line number
			YOU HAVE TO NAME YOUR LINES AND YOU HAVE TO NAME THEM DIFFERENTLY */

			// Create a tablefor array data
			vtkSmartPointer<vtkTable> table = vtkSmartPointer<vtkTable>::New();

			// Instantiate an array of vtkFloatArray type. (Holds line data )
			vtkSmartPointer<vtkFloatArray> arr[2 * numDataSets];

			// Allocate float array data and give names. Note numLines + 1 as you have to include x-axis. 
			for (int j = 0; j < 2 * numDataSets; j++) {

				// Allocate float array data
				arr[j] = vtkSmartPointer<vtkFloatArray>::New();

				// NAMING LINES MATTERS BECAUSE IF YOU DONT VTK WONT WORK. THEY ALSO NEED DIFFERENT NAMES. 
				arr[j]->SetName(names[j].c_str());

				// Add collumn to vtkTable with vtkFloatarray memory 
				table->AddColumn(arr[j]);
			}

			// Set number of rows on table for number of points 
			table->SetNumberOfRows(numPoints);

			// Set/transform raw data to vtkTable (rows -> pts, cols -> Lines : The way VTK data works..) 
			for (int i = 0; i < numDataSets; i++) {
				for (int j = 0; j < numPoints; j++) {

					table->SetValue(j, i, x_pos[i][j]);
					table->SetValue(j, i + numDataSets, data[i][j]);	// Accss data col-wise for efficiency (col0,col1,col2,col3 etc..) 
				}
			}

			// Set up the view
			vtkSmartPointer<vtkContextView> view = vtkSmartPointer<vtkContextView>::New();
			view->GetRenderer()->SetBackground(1.0, 1.0, 1.0);

			// Add multiple line plots, setting the colors etc
			vtkSmartPointer<vtkChartXY> chart = vtkSmartPointer<vtkChartXY>::New();
			view->GetScene()->AddItem(chart);

			// Instantiate a vtkPlot object with POINTS
			vtkPlot* points = chart->AddPlot(vtkChart::POINTS);

			// RGB array for random colours for plotting multiple datasets
			double RGB[3];

			// Loop over lines and input data to vtkLine for plotting: Line -> chart -> view -> Renderwindow, Interactor
			for (int j = 0; j < numDataSets; j++) {

				// Add a plot for each line after first iteration. 
				if (j > 0) {
					points = chart->AddPlot(vtkChart::POINTS);
				}
				points->SetInputData(table, j, j + numDataSets);

				// Line properties
				RGB[0] = std::rand() % 255;
				RGB[1] = std::rand() % 255;
				RGB[2] = std::rand() % 255;
				points->SetColor(RGB[0], RGB[1], RGB[2]);
				points->SetWidth(1.0);
				dynamic_cast<vtkPlotPoints*>(points)->SetMarkerStyle(vtkPlotPoints::CIRCLE); // CIRCLE = 4 in shape enum in vtkPlotPoints class

				// Dynamic_cast for runtime checks on polymorphism 
				// Polymorphism used as chart does not take vtkPlotPoints as an input (I think)
				// e.g. setMarkerStyle not available in vtkPlot but is in vtkPlotPoints. 
			}

			// Start interactor
			view->GetRenderWindow()->Render();
			view->GetInteractor()->Initialize();
			view->GetInteractor()->Start();
		}

		/* =================================================================================================================
		Plotting single lines on the same render window (Lines not together in memory (e.g. data_set_1[10], data_set_2[10]))
		Note: Works with different X coordinates for each data set.
		================================================================================================================= */

		/* Enum containing various marker styles that can be used in a plot */
		enum {
			NONE,CROSS,PLUS,SQUARE,CIRCLE,DIAMOND
		};

		// Plotting single scatter plots on same render window 
		template<int numPoints> 
		void Scatter_plotter(vtkSmartPointer<vtkChartXY>& chart, float(&x_pos)[numPoints], float(&data)[numPoints], std::string& name, const char* PointColour, float width, int marker) {

			/* ----- Notes -----:
			Input data		-> rows = Line number, cols = points on lines
			vtKtable data	-> rows = points on lines, cols = Line number
			YOU HAVE TO NAME YOUR LINES AND YOU HAVE TO NAME THEM DIFFERENTLY
			*/

			// Create a table with some points in it
			vtkSmartPointer<vtkTable> table = vtkSmartPointer<vtkTable>::New();

			// Instantiate an array of vtkFloatArray type. (Holds line data )
			vtkSmartPointer<vtkFloatArray> arr[2];

			// Instantiate the x-axis (Cant use condition in allocation loop for some reason...)
			arr[X_axis] = vtkSmartPointer<vtkFloatArray>::New();
			arr[X_axis]->SetName("X-axis");
			table->AddColumn(arr[X_axis]);

			// Instantiate data for next line: 
			// Allocate float array data and give name
			arr[1] = vtkSmartPointer<vtkFloatArray>::New();			// Allocate float array data
			arr[1]->SetName(name.c_str());									// Set name
			table->AddColumn(arr[1]);								// Add collumn to vtkTable with vtkFloatarray memory 

			// Set number of rows on table for number of points 
			table->SetNumberOfRows(numPoints);

			// Set/transform raw data to vtkTable 

			for (int i = 0; i < numPoints; i++) {
				table->SetValue(i, 0, x_pos[i]);		// Recall x axis is vtkTable col [0].
				table->SetValue(i, 1, data[i]);
			}


			// vtkNamedColors:  A class holding colors and their names.
			// For info see the folder with VTK_Coloursheets for different colour names. 
			vtkSmartPointer<vtkNamedColors> colors = vtkSmartPointer<vtkNamedColors>::New();

			// Instantiate a vtkLine object 
			vtkPlot* points = chart->AddPlot(vtkChart::POINTS);
			points->SetInputData(table, 0, 1);
			points->GetPen()->SetColorF(colors->GetColor3d(PointColour).GetData());
			points->SetWidth(width);
			dynamic_cast<vtkPlotPoints*>(points)->SetMarkerStyle(marker);
		}

		/*=================================================================================================================
		Plot single point in 2D.
		=================================================================================================================== */
		void Scatter_plotter(vtkSmartPointer<vtkChartXY>& chart, float x_pos, float y_pos, std::string& name, const char* PointColour, float width, int marker) {

			/* ----- Notes -----:
			Input data		-> rows = Line number, cols = points on lines
			vtKtable data	-> rows = points on lines, cols = Line number
			YOU HAVE TO NAME YOUR LINES AND YOU HAVE TO NAME THEM DIFFERENTLY
			*/

			// Create a table with some points in it
			vtkSmartPointer<vtkTable> table = vtkSmartPointer<vtkTable>::New();

			// Instantiate an array of vtkFloatArray type. (Holds line data )
			vtkSmartPointer<vtkFloatArray> arr[2];

			// Instantiate the x-axis (Cant use condition in allocation loop for some reason...)
			arr[X_axis] = vtkSmartPointer<vtkFloatArray>::New();
			arr[X_axis]->SetName("X-axis");
			table->AddColumn(arr[X_axis]);

			// Instantiate data for next line: 
			// Allocate float array data and give name
			arr[1] = vtkSmartPointer<vtkFloatArray>::New();			// Allocate float array data
			arr[1]->SetName(name.c_str());									// Set name
			table->AddColumn(arr[1]);								// Add collumn to vtkTable with vtkFloatarray memory 

			// Set number of rows on table for number of points 
			table->SetNumberOfRows(1);

			// Set/transform raw data to vtkTable 
			table->SetValue(0, 0, x_pos);		
			table->SetValue(0, 1, y_pos);
			

			// vtkNamedColors:  A class holding colors and their names.
			// For info see the folder with VTK_Coloursheets for different colour names. 
			vtkSmartPointer<vtkNamedColors> colors = vtkSmartPointer<vtkNamedColors>::New();

			// Instantiate a vtkLine object 
			vtkPlot* points = chart->AddPlot(vtkChart::POINTS);
			points->SetInputData(table, 0, 1);
			points->GetPen()->SetColorF(colors->GetColor3d(PointColour).GetData());
			points->SetWidth(width);
			dynamic_cast<vtkPlotPoints*>(points)->SetMarkerStyle(marker);
		}

		//	// Dynamic memory variant(std::vector) 
		//	void _2DLine_plotter(const int numPoints, float inc, std::vector<float>& x_pos, std::vector<float>& data_1, std::vector<float>& data_2) {
		//
		//
		//		// Create a table with some points in it
		//		vtkSmartPointer<vtkTable> table = vtkSmartPointer<vtkTable>::New();
		//
		//		vtkSmartPointer<vtkFloatArray> arrX = vtkSmartPointer<vtkFloatArray>::New();
		//		arrX->SetName("X Axis");
		//		table->AddColumn(arrX);
		//
		//		vtkSmartPointer<vtkFloatArray> arrC = vtkSmartPointer<vtkFloatArray>::New();
		//		arrC->SetName("Cosine");
		//		table->AddColumn(arrC);
		//
		//		vtkSmartPointer<vtkFloatArray> arrS = vtkSmartPointer<vtkFloatArray>::New();
		//		arrS->SetName("Sine");
		//		table->AddColumn(arrS);
		//		table->SetNumberOfRows(numPoints);
		//
		//		// Transform raw data to vtkTable 
		//		for (int i = 0; i < numPoints; i++) {
		//			table->SetValue(i, 0, x_pos[i]);
		//			table->SetValue(i, 1, data_1[i]);
		//			table->SetValue(i, 2, data_2[i]);
		//		}
		//
		//		// Set up the view
		//		vtkSmartPointer<vtkContextView> view =
		//			vtkSmartPointer<vtkContextView>::New();
		//		view->GetRenderer()->SetBackground(1.0, 1.0, 1.0);
		//
		//		// Add multiple line plots, setting the colors etc
		//		vtkSmartPointer<vtkChartXY> chart =
		//			vtkSmartPointer<vtkChartXY>::New();
		//		view->GetScene()->AddItem(chart);
		//		vtkPlot* line = chart->AddPlot(vtkChart::LINE);
		//		line->SetInputData(table, 0, 1);
		//		line->SetColor(0, 255, 0, 255);
		//		line->SetWidth(1.0);
		//		line = chart->AddPlot(vtkChart::LINE);
		//		line->SetInputData(table, 0, 2);
		//		line->SetColor(255, 0, 0, 255);
		//		line->SetWidth(5.0);
		//
		//		// For dotted line, the line type can be from 2 to 5 for different dash/dot
		//		// patterns (see enum in vtkPen containing DASH_LINE, value 2):
		//#ifndef WIN32
		//		line->GetPen()->SetLineType(vtkPen::DASH_LINE);
		//#endif
		//		// (ifdef-ed out on Windows because DASH_LINE does not work on Windows
		//		//  machines with built-in Intel HD graphics card...)
		//
		//		//view->GetRenderWindow()->SetMultiSamples(0);
		//
		//		// Start interactor
		//		view->GetRenderWindow()->Render();
		//		view->GetInteractor()->Initialize();
		//		view->GetInteractor()->Start();
//	}
	}
}
