/* ==================================================================================================
 -------------------------- 3D Line plotter wrapper for the VTK library -----------------------------
 ==================================================================================================*/

// Include library interface files
#include <vtkNew.h>
#include <vtkSmartPointer.h>
#include <vtkChartXYZ.h>
#include <vtkPen.h>
#include <vtkPlotLine3D.h>
#include <vtkAxis.h>
#include <vtkNamedColors.h>
#include <vtkRenderWindow.h>
#include <vtkTable.h>
#include <vtkFloatArray.h>
#include <vtkContextView.h>
#include <vtkContextScene.h>
#include <vtkContext3D.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkCamera.h>

// External modules 
# include <iostream>

namespace W_VTK {
	namespace _3D {
		/* Enum for code clarity */
		enum {
			X_axis, Y_axis, Z_axis, spatial_dimensions
		};

		/* =-=-=-=-=-=-=-=-=-=-=-==-=-=-=-=-=-=-=-=-=-=-==-=-=-=-=-=-=-=-=-=-=-==-=-=-=-=-=-=-=-=-=-=-==-=-=-=-=-=-=-=-=-=-=-= */
		/* ------------------- 3D Line plotters: Static memory variant (memory must be known at compile time) ---------------- */
		/* =-=-=-=-=-=-=-=-=-=-=-==-=-=-=-=-=-=-=-=-=-=-==-=-=-=-=-=-=-=-=-=-=-==-=-=-=-=-=-=-=-=-=-=-==-=-=-=-=-=-=-=-=-=-=-= */

		/*=================================================================================================================
		Plot single line in 3D.
		=================================================================================================================== */
		template <int numPoints>
		void Line_plotter(float(&data)[spatial_dimensions][numPoints], const char* LineColourName, const char* BackgroundColour, float width) {

			// vtkNamedColors:  A class holding colors and their names.
			// For info see the folder with VTK_Coloursheets for different colour names. 
			vtkSmartPointer<vtkNamedColors> colors = vtkSmartPointer<vtkNamedColors>::New();

			// Create the data.
			vtkSmartPointer<vtkTable> table = vtkSmartPointer<vtkTable>::New();

			vtkSmartPointer<vtkFloatArray> arr[spatial_dimensions];	//	3- Dimensions X,Y,Z 

			// Allocate float array data for VTK data structures, name and add to vtkTable. 
			arr[X_axis] = vtkSmartPointer<vtkFloatArray>::New();
			arr[X_axis]->SetName("X");
			table->AddColumn(arr[0]);

			arr[Y_axis] = vtkSmartPointer<vtkFloatArray>::New();
			arr[Y_axis]->SetName("Y");
			table->AddColumn(arr[1]);

			arr[Z_axis] = vtkSmartPointer<vtkFloatArray>::New();
			arr[Z_axis]->SetName("Z");
			table->AddColumn(arr[2]);

			// Set number of rows on table for number of points 
			table->SetNumberOfRows(numPoints);

			// Set/transform raw data to vtk data structure
			for (unsigned int i = 0; i < spatial_dimensions; i++) {	// Dimensions loop 
				for (unsigned int j = 0; j < numPoints; j++) {		// Discretization loop (points) 
					table->SetValue(j, i, data[i][j]);				// X -> j = 0; Y -> j = 1; Z -> j = 2
				}
			}

			// Set up a 3D scene and add an XYZ chart to it.
			vtkSmartPointer<vtkContextView> view = vtkSmartPointer<vtkContextView>::New();
			view->GetRenderWindow()->SetSize(640, 480);

			// Set up a chart to contrain the plots. 
			vtkSmartPointer<vtkChartXYZ> chart = vtkSmartPointer<vtkChartXYZ>::New();

			// Set the geometry of the 3D scene (axes box)
			chart->SetGeometry(vtkRectf(5.0f, 5.0f, 635.0f, 475.0f));	// Numbers which position (pixels) and determine the axis's on the render window

			// Create a 3D line plot using vtkPlotLine3D and input vtkTable data
			// Create multiple of these for multiple 3D lines !! =================
			vtkSmartPointer<vtkPlotLine3D> plot = vtkSmartPointer<vtkPlotLine3D>::New();
			plot->SetInputData(table);

			// vtkPen:  provides a pen that draws the outlines of shapes drawn
			// -> GetPen() is part of the parent class for vtkPlotLine3D and the "Pen" controls how the plot draws (e.g. properties such as colour.) 
			plot->GetPen()->SetColorF(colors->GetColor3d(LineColourName).GetData());

			// view->GetRenderWindow()->SetMultiSamples(0);
			plot->GetPen()->SetWidth(width);

			// Add the plot to the chart 
			chart->AddPlot(plot);


			//chart->GetAxis(0)->SetOpacity(0.0); 
			// Set axis colour same as background to get rid of axis. 
			// SetAxisColor takes input of class Color4ub...
			//chart->SetAxisColor(colors->GetColor4ub("zinc_white"));

			//chart->GetAxis(0)->SetAxisVisible(false);
			//chart->GetAxis(1)->SetAxisVisible(false);
			//chart->GetAxis(2)->SetAxisVisible(false);

			//chart->GetAxis(0)->SetLabelsVisible(false);
			//chart->GetAxis(1)->SetLabelsVisible(false);
			//chart->GetAxis(2)->SetLabelsVisible(false);

		
			//std::cout << "X axis visibility bool in chart: " << chart->GetAxis(0)->GetAxisVisible() << "\n";
			//std::cout << "Y axis visibility bool in chart: " << chart->GetAxis(1)->GetAxisVisible() << "\n";
			//std::cout << "Z axis visibility bool in chart: " << chart->GetAxis(2)->GetAxisVisible() << "\n";
			//std::cin.get();

			// Apply chart data structure to scene
			view->GetScene()->AddItem(chart);

			// Render the scene
			view->GetRenderer()->SetBackground(colors->GetColor3d(BackgroundColour).GetData());
			view->GetRenderWindow()->Render();
			view->GetInteractor()->Initialize();
			view->GetInteractor()->Start();
		}


		/* =================================================================================================================
		Plotting single 3D lines on the same render window (Lines not together in memory (e.g. data_set_1[3][10], data_set_2[3][10]))
		Note: Works with different X coordinates for each data set.
		================================================================================================================= */
		template <int numPoints>
		void Line_plotter(vtkSmartPointer<vtkChartXYZ>& chart, float(&data)[spatial_dimensions][numPoints], const char* LineColourName, float width) {

			// vtkNamedColors:  A class holding colors and their names.
			// For info see the folder with VTK_Coloursheets for different colour names. 
			vtkSmartPointer<vtkNamedColors> colors = vtkSmartPointer<vtkNamedColors>::New();

			// Create the data.
			vtkSmartPointer<vtkTable> table = vtkSmartPointer<vtkTable>::New();

			vtkSmartPointer<vtkFloatArray> arr[spatial_dimensions];	//	3- Dimensions X,Y,Z 

			// Allocate float array data for VTK data structures, name and add to vtkTable. 
			arr[X_axis] = vtkSmartPointer<vtkFloatArray>::New();
			arr[X_axis]->SetName("X");
			table->AddColumn(arr[0]);

			arr[Y_axis] = vtkSmartPointer<vtkFloatArray>::New();
			arr[Y_axis]->SetName("Y");
			table->AddColumn(arr[1]);

			arr[Z_axis] = vtkSmartPointer<vtkFloatArray>::New();
			arr[Z_axis]->SetName("Z");
			table->AddColumn(arr[2]);

			// Set number of rows on table for number of points 
			table->SetNumberOfRows(numPoints);

			// Set/transform raw data to vtk data structure
			for (unsigned int i = 0; i < spatial_dimensions; i++) {	// Dimensions loop 
				for (unsigned int j = 0; j < numPoints; j++) {		// Discretization loop (points) 
					table->SetValue(j, i, data[i][j]);				// X -> j = 0; Y -> j = 1; Z -> j = 2
				}
			}

			// Create a 3D line plot using vtkPlotLine3D and input vtkTable data
			// Create multiple of these for multiple 3D lines !! =================
			vtkSmartPointer<vtkPlotLine3D> plot = vtkSmartPointer<vtkPlotLine3D>::New();
			plot->SetInputData(table);

			// vtkPen:  provides a pen that draws the outlines of shapes drawn
			// -> GetPen() is part of the parent class for vtkPlotLine3D and the "Pen" controls how the plot draws (e.g. properties such as colour.) 
			plot->GetPen()->SetColorF(colors->GetColor3d(LineColourName).GetData());

			// view->GetRenderWindow()->SetMultiSamples(0);
			plot->GetPen()->SetWidth(width);

			// Add the plot to the chart 
			chart->AddPlot(plot);


			//chart->GetAxis(0)->SetOpacity(0.0); 
			// Set axis colour same as background to get rid of axis. 
			// SetAxisColor takes input of class Color4ub...
			//chart->SetAxisColor(colors->GetColor4ub("zinc_white"));

			//chart->GetAxis(0)->SetAxisVisible(false);
			//chart->GetAxis(1)->SetAxisVisible(false);
			//chart->GetAxis(2)->SetAxisVisible(false);

			//std::cout << "X axis visibility bool in chart: " << chart->GetAxis(0)->GetAxisVisible() << "\n";
			//std::cout << "Y axis visibility bool in chart: " << chart->GetAxis(1)->GetAxisVisible() << "\n";
			//std::cout << "Z axis visibility bool in chart: " << chart->GetAxis(2)->GetAxisVisible() << "\n";
			//std::cin.get();
		}

		
		// Function to instatiate a vtkChartXYZ data structure for containing line plot data
		vtkSmartPointer<vtkChartXYZ> multiplot_chart_instantiation() {

			// Set up a chart to contrain the plots. 
			vtkSmartPointer<vtkChartXYZ> chart = vtkSmartPointer<vtkChartXYZ>::New();

			// Set the geometry of the 3D scene (axes box)
			chart->SetGeometry(vtkRectf(5.0f, 5.0f, 635.0f, 475.0f));	// Numbers which position (pixels) and determine the axis's on the render window

			//vtkSmartPointer<vtkAxis> axesX;
			//axesX->SetAxisVisible(false);
			//chart->SetAxis(0, axesX);

			return chart;
		}

		// Function to start the render window and interactor. 
		void multiplot_view_window(vtkSmartPointer<vtkChartXYZ>& chart, const char* BackgroundColour) {

			// vtkNamedColors:  A class holding colors and their names.
			// For info see the folder with VTK_Coloursheets for different colour names. 
			vtkSmartPointer<vtkNamedColors> colors = vtkSmartPointer<vtkNamedColors>::New();

			// Set up a 3D scene and add an XYZ chart to it.
			vtkSmartPointer<vtkContextView> view = vtkSmartPointer<vtkContextView>::New();
			view->GetRenderWindow()->SetSize(640, 480);

			// Add the chart to the view
			view->GetScene()->AddItem(chart);

			// Render the scene
			view->GetRenderer()->SetBackground(colors->GetColor3d(BackgroundColour).GetData());
			view->GetRenderWindow()->Render();
			view->GetInteractor()->Initialize();
			view->GetInteractor()->Start();
		}

		// Note, no option to show legend or name the lines for vtkChartXYZ (no member functions like in vtkChartXY)


		/* =-=-=-=-=-=-=-=-=-=-=-==-=-=-=-=-=-=-=-=-=-=-==-=-=-=-=-=-=-=-=-=-=-==-=-=-=-=-=-=-=-=-=-=-==-=-=-=-=-=-=-=-=-=-=-= */
		/* ----------------- 3D scatter plotters: Static memory variant (memory must be known at compile time) ---------------- */
		/* =-=-=-=-=-=-=-=-=-=-=-==-=-=-=-=-=-=-=-=-=-=-==-=-=-=-=-=-=-=-=-=-=-==-=-=-=-=-=-=-=-=-=-=-==-=-=-=-=-=-=-=-=-=-=-= */

		/*=================================================================================================================
		Plot single scatter plot in 3D.
		=================================================================================================================== */
		template <int numPoints>
		void Scatter_plotter(float(&data)[spatial_dimensions][numPoints], const char* PointColourName, const char* BackgroundColour, float width ) {

			// vtkNamedColors:  A class holding colors and their names.
			// For info see the folder with VTK_Coloursheets for different colour names. 
			vtkSmartPointer<vtkNamedColors> colors = vtkSmartPointer<vtkNamedColors>::New();

			// Create the data.
			vtkSmartPointer<vtkTable> table = vtkSmartPointer<vtkTable>::New();

			vtkSmartPointer<vtkFloatArray> arr[spatial_dimensions];	//	3- Dimensions X,Y,Z 

			// Allocate float array data for VTK data structures, name and add to vtkTable. 
			arr[X_axis] = vtkSmartPointer<vtkFloatArray>::New();
			arr[X_axis]->SetName("X");
			table->AddColumn(arr[0]);

			arr[Y_axis] = vtkSmartPointer<vtkFloatArray>::New();
			arr[Y_axis]->SetName("Y");
			table->AddColumn(arr[1]);

			arr[Z_axis] = vtkSmartPointer<vtkFloatArray>::New();
			arr[Z_axis]->SetName("Z");
			table->AddColumn(arr[2]);

			// Set number of rows on table for number of points 
			table->SetNumberOfRows(numPoints);

			// Set/transform raw data to vtk data structure
			for (unsigned int i = 0; i < spatial_dimensions; i++) {	// Dimensions loop 
				for (unsigned int j = 0; j < numPoints; j++) {		// Discretization loop (points) 
					table->SetValue(j, i, data[i][j]);				// X -> j = 0; Y -> j = 1; Z -> j = 2
				}
			}

			// Set up a 3D scene and add an XYZ chart to it.
			vtkSmartPointer<vtkContextView> view = vtkSmartPointer<vtkContextView>::New();
			view->GetRenderWindow()->SetSize(640, 480);

			// Set up a chart to contrain the plots. 
			vtkSmartPointer<vtkChartXYZ> chart = vtkSmartPointer<vtkChartXYZ>::New();

			// Set the geometry of the 3D scene (axes box)
			chart->SetGeometry(vtkRectf(5.0f, 5.0f, 635.0f, 475.0f));	// Numbers which position (pixels) and determine the axis's on the render window

			// Create a 3D line plot using vtkPlotLine3D and input vtkTable data
			// Create multiple of these for multiple 3D lines !! =================
			vtkSmartPointer<vtkPlotPoints3D> plot = vtkSmartPointer<vtkPlotPoints3D>::New();
			plot->SetInputData(table);

			// vtkPen:  provides a pen that draws the outlines of shapes drawn
			// -> GetPen() is part of the parent class for vtkPlotLine3D and the "Pen" controls how the plot draws (e.g. properties such as colour.) 
			plot->GetPen()->SetColorF(colors->GetColor3d(PointColourName).GetData());

			// view->GetRenderWindow()->SetMultiSamples(0);
			plot->GetPen()->SetWidth(width);

			// Add the plot to the chart 
			chart->AddPlot(plot);


			//chart->GetAxis(0)->SetOpacity(0.0); 
			// Set axis colour same as background to get rid of axis. 
			// SetAxisColor takes input of class Color4ub...
			//chart->SetAxisColor(colors->GetColor4ub("zinc_white"));

			//chart->GetAxis(0)->SetAxisVisible(false);
			//chart->GetAxis(1)->SetAxisVisible(false);
			//chart->GetAxis(2)->SetAxisVisible(false);

			//chart->GetAxis(0)->SetLabelsVisible(false);
			//chart->GetAxis(1)->SetLabelsVisible(false);
			//chart->GetAxis(2)->SetLabelsVisible(false);


			//std::cout << "X axis visibility bool in chart: " << chart->GetAxis(0)->GetAxisVisible() << "\n";
			//std::cout << "Y axis visibility bool in chart: " << chart->GetAxis(1)->GetAxisVisible() << "\n";
			//std::cout << "Z axis visibility bool in chart: " << chart->GetAxis(2)->GetAxisVisible() << "\n";
			//std::cin.get();

			// Apply chart data structure to scene
			view->GetScene()->AddItem(chart);

			// Render the scene
			view->GetRenderer()->SetBackground(colors->GetColor3d(BackgroundColour).GetData());
			view->GetRenderWindow()->Render();
			view->GetInteractor()->Initialize();
			view->GetInteractor()->Start();
		}

		/* =================================================================================================================
		Plotting single 3D scatter plot on the same render window (Lines not together in memory (e.g. data_set_1[3][10], data_set_2[3][10]))
		Note: Works with different X coordinates for each data set.
		================================================================================================================= */
		template <int numPoints>
		void Scatter_plotter(vtkSmartPointer<vtkChartXYZ>& chart, float(&data)[spatial_dimensions][numPoints], const char* PointColourName, float width) {

			// vtkNamedColors:  A class holding colors and their names.
			// For info see the folder with VTK_Coloursheets for different colour names. 
			vtkSmartPointer<vtkNamedColors> colors = vtkSmartPointer<vtkNamedColors>::New();

			// Create the data.
			vtkSmartPointer<vtkTable> table = vtkSmartPointer<vtkTable>::New();

			vtkSmartPointer<vtkFloatArray> arr[spatial_dimensions];	//	3- Dimensions X,Y,Z 

			// Allocate float array data for VTK data structures, name and add to vtkTable. 
			arr[X_axis] = vtkSmartPointer<vtkFloatArray>::New();
			arr[X_axis]->SetName("X");
			table->AddColumn(arr[0]);

			arr[Y_axis] = vtkSmartPointer<vtkFloatArray>::New();
			arr[Y_axis]->SetName("Y");
			table->AddColumn(arr[1]);

			arr[Z_axis] = vtkSmartPointer<vtkFloatArray>::New();
			arr[Z_axis]->SetName("Z");
			table->AddColumn(arr[2]);

			// Set number of rows on table for number of points 
			table->SetNumberOfRows(numPoints);

			// Set/transform raw data to vtk data structure
			for (unsigned int i = 0; i < spatial_dimensions; i++) {	// Dimensions loop 
				for (unsigned int j = 0; j < numPoints; j++) {		// Discretization loop (points) 
					table->SetValue(j, i, data[i][j]);				// X -> j = 0; Y -> j = 1; Z -> j = 2
				}
			}

			// Create a 3D line plot using vtkPlotLine3D and input vtkTable data
			// Create multiple of these for multiple 3D lines !! =================
			vtkSmartPointer<vtkPlotPoints3D> plot = vtkSmartPointer<vtkPlotPoints3D>::New();
			plot->SetInputData(table);

			// vtkPen:  provides a pen that draws the outlines of shapes drawn
			// -> GetPen() is part of the parent class for vtkPlotLine3D and the "Pen" controls how the plot draws (e.g. properties such as colour.) 
			plot->GetPen()->SetColorF(colors->GetColor3d(PointColourName).GetData());

			// view->GetRenderWindow()->SetMultiSamples(0);
			plot->GetPen()->SetWidth(width);

			// Add the plot to the chart 
			chart->AddPlot(plot);


			//chart->GetAxis(0)->SetOpacity(0.0); 
			// Set axis colour same as background to get rid of axis. 
			// SetAxisColor takes input of class Color4ub...
			//chart->SetAxisColor(colors->GetColor4ub("zinc_white"));

			//chart->GetAxis(0)->SetAxisVisible(false);
			//chart->GetAxis(1)->SetAxisVisible(false);
			//chart->GetAxis(2)->SetAxisVisible(false);

			//std::cout << "X axis visibility bool in chart: " << chart->GetAxis(0)->GetAxisVisible() << "\n";
			//std::cout << "Y axis visibility bool in chart: " << chart->GetAxis(1)->GetAxisVisible() << "\n";
			//std::cout << "Z axis visibility bool in chart: " << chart->GetAxis(2)->GetAxisVisible() << "\n";
			//std::cin.get();
		}


		/*=================================================================================================================
		Plot single point in 3D.
		=================================================================================================================== */

		void Scatter_plotter(vtkSmartPointer<vtkChartXYZ>& chart, float(&data)[spatial_dimensions], const char* PointColourName, float width) {

			// vtkNamedColors:  A class holding colors and their names.
			// For info see the folder with VTK_Coloursheets for different colour names. 
			vtkSmartPointer<vtkNamedColors> colors = vtkSmartPointer<vtkNamedColors>::New();

			// Create the data.
			vtkSmartPointer<vtkTable> table = vtkSmartPointer<vtkTable>::New();

			vtkSmartPointer<vtkFloatArray> arr[spatial_dimensions];	//	3- Dimensions X,Y,Z 

			// Allocate float array data for VTK data structures, name and add to vtkTable. 
			arr[X_axis] = vtkSmartPointer<vtkFloatArray>::New();
			arr[X_axis]->SetName("X");
			table->AddColumn(arr[0]);

			arr[Y_axis] = vtkSmartPointer<vtkFloatArray>::New();
			arr[Y_axis]->SetName("Y");
			table->AddColumn(arr[1]);

			arr[Z_axis] = vtkSmartPointer<vtkFloatArray>::New();
			arr[Z_axis]->SetName("Z");
			table->AddColumn(arr[2]);

			// Set number of rows on table for number of points 
			table->SetNumberOfRows(1);

			// Set/transform raw data to vtk data structure
			for (unsigned int i = 0; i < spatial_dimensions; i++) {	// Dimensions loop 
				table->SetValue(0, i, data[i]);				
			}

			// Create a 3D line plot using vtkPlotLine3D and input vtkTable data
			// Create multiple of these for multiple 3D lines !! =================
			vtkSmartPointer<vtkPlotPoints3D> plot = vtkSmartPointer<vtkPlotPoints3D>::New();
			plot->SetInputData(table);

			// vtkPen:  provides a pen that draws the outlines of shapes drawn
			// -> GetPen() is part of the parent class for vtkPlotLine3D and the "Pen" controls how the plot draws (e.g. properties such as colour.) 
			plot->GetPen()->SetColorF(colors->GetColor3d(PointColourName).GetData());

			// view->GetRenderWindow()->SetMultiSamples(0);
			plot->GetPen()->SetWidth(width);

			// Add the plot to the chart 
			chart->AddPlot(plot);


			//chart->GetAxis(0)->SetOpacity(0.0); 
			// Set axis colour same as background to get rid of axis. 
			// SetAxisColor takes input of class Color4ub...
			//chart->SetAxisColor(colors->GetColor4ub("zinc_white"));

			//chart->GetAxis(0)->SetAxisVisible(false);
			//chart->GetAxis(1)->SetAxisVisible(false);
			//chart->GetAxis(2)->SetAxisVisible(false);

			//std::cout << "X axis visibility bool in chart: " << chart->GetAxis(0)->GetAxisVisible() << "\n";
			//std::cout << "Y axis visibility bool in chart: " << chart->GetAxis(1)->GetAxisVisible() << "\n";
			//std::cout << "Z axis visibility bool in chart: " << chart->GetAxis(2)->GetAxisVisible() << "\n";
			//std::cin.get();
		}
	}
	
}
