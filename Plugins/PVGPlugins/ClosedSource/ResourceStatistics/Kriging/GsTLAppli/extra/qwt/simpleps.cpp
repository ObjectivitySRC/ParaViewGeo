/**********************************************************************
** Author: Ting Li
** Copyright (C) 2002-2004 The Board of Trustees of the Leland Stanford Junior
**   University
** All rights reserved.
**
** This file is part of the "gui" module of the Geostatistical Earth
** Modeling Software (GEMS)
**
** This file may be distributed and/or modified under the terms of the 
** license defined by the Stanford Center for Reservoir Forecasting and 
** appearing in the file LICENSE.XFREE included in the packaging of this file.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.gnu.org/copyleft/gpl.html for GPL licensing information.
**
** Contact the Stanford Center for Reservoir Forecasting, Stanford University
** if any conditions of this licensing are not clear to you.
**
**********************************************************************/


#include <GsTLAppli/extra/qwt/simpleps.h>
#include <stdio.h>  // for sprintf

#define MAJOR 6
#define MINOR 5
#define DIGITS 2
#define AXISLEN 250

#include <GsTLAppli/extra/qwt/qwt_math.h>
#include <GsTLAppli/extra/qwt/qwt_plot.h>
#include <assert.h>
#include <math.h>   //for log
#include <fstream>
#include <vector>

typedef std::vector< std::pair<std::string,std::string> >::iterator Itr;

SimplePs::SimplePs(std::string fn, QwtPlot * p, Stat & s, bool d) : 
	_o((const char *)fn.c_str()), _plot(p), _stats(s), _drawStats(d)
{	
	if (_o.is_open()) 
		init();
}

SimplePs::SimplePs(std::string fn, QwtPlot * p) : 
	_o((const char *)fn.c_str()), _plot(p)
{	
	if (_o.is_open())
		init();
	_drawStats = false;
}
bool SimplePs::isopen()
{
	return _o.is_open();
}

void SimplePs::init()
{
	_o << "%!PS-Adobe EPSF-3.0\n";  //generate EPS
	_o << "%Creator: S-GeMS\n";
	if (!_drawStats)
		_o << "%%BoundingBox: 30 166 501 613\n";
	else
		_o << "%%BoundingBox: 37 166 501 620\n";

	if (_stats.empty())
		_o << "90 234 translate 1.5 1.5 scale\n";
	else
		_o << "90 234 translate 1.3 1.5 scale\n";

	_o << "/m {moveto} def /l {lineto} def /r {rlineto} def\n";
	_o << "/s {stroke} def /n {newpath} def /c {closepath} def\n";
	_o << "/rtext{ dup stringwidth pop -1 div 0 rmoveto show } def\n";
	_o << "/ctext{ dup stringwidth pop -2 div 0 rmoveto show } def\n";
	_o << "/ltext{show} def /gr{grestore} def /gs{gsave} def\n";
	_o << "/tr{translate} def /setc{setrgbcolor} def\n";
	_o << "/bullet{ 2 0 360 arc c fill } def\n";
	_o << "/parea{n 0 0 m\n 0 " << AXISLEN << " l\n";
	_o << AXISLEN << " " << AXISLEN << " l\n" << AXISLEN << " 0 l\n";
	_o << "c\n} def\n";

}

void SimplePs::drawLine(int x1, int y1, int x2, int y2, float linewidth)
{
	_o << "n\n" << x1 << " " << y1 << " m\n";
	_o << x2 << " " << y2 << " r\n" << linewidth << " setlinewidth\ns\n";
}

void SimplePs::drawTic(int x, int y, bool vert, bool major, bool right)
{
	int len;
	_o << "n\n" << x << " " << y << " m\n" << 0.5 << " setlinewidth\n";

	if (major)
		len = 8;
	else
		len = 4;

	if (vert)
		_o << "0 " << -len << " r\ns\n";
	else {
		if (!right)
			_o << -len << " 0 r\ns\n";
		else
			_o << len << " 0 r\ns\n";
	}
}


void SimplePs::drawText(int x, int y, std::string font, int scale, int rotate,
						std::string text)
{
	_o << "/" << font << " findfont\ngsave\n";
	_o << scale << " scalefont\nsetfont\n";
	_o << rotate << " rotate\n";
	_o << "n\n" << x << " " << y << " m\n(" << text << ") show\ngrestore\n";
}

void SimplePs::drawCurve(QArray<double>  & x, QArray<double> & y,int style)
{
	drawCurve(x,y,style,false);
}

void SimplePs::drawCurve(QArray<double>  x, QArray<double> y,int style, bool right)
{
	int i;
	double xmin,xmax,ymin,ymax,xscale,yscale,width,xlogscale,ylogscale;
	
	xmin = _plot->axisScale(QwtPlot::xBottom)->lBound();
	xmax = _plot->axisScale(QwtPlot::xBottom)->hBound();
	if (!right) {
		ymin = _plot->axisScale(QwtPlot::yLeft)->lBound();
		ymax = _plot->axisScale(QwtPlot::yLeft)->hBound();
	}
	else {
		ymin = _plot->axisScale(QwtPlot::yRight)->lBound();
		ymax = _plot->axisScale(QwtPlot::yRight)->hBound();
	}

	if (_plot->axisScale( QwtPlot::xBottom )->logScale() == true) {
		for (i = 0; i < x.size(); ++i)
			if (x[i] > 0)
				x[i] = log10(x[i]);
		xmin = log10(xmin);
		xmax = log10(xmax);
	}
	if (_plot->axisScale( QwtPlot::yLeft )->logScale() == true) {
		for (i = 0; i < y.size(); ++i)
			if (y[i] > 0)
				y[i] = log10(y[i]);
		ymin = log10(ymin);
		ymax = log10(ymax);
	}
	xscale = (double)AXISLEN/(xmax-xmin);
	yscale = (double)AXISLEN/(ymax-ymin);
	width = (x[2]-x[1])*xscale;


	_o << "gs\n parea clip\n";
	double x1,y1;

	if (style == QwtCurve::Lines) {
		_o << "n\n 0 0 m\n";
		for (i = 0; i < x.size(); ++i) {
			x1 = (x[i]-xmin)*xscale;
			y1 = (y[i]-ymin)*yscale;

			_o << x1 << " " << y1 << " lineto\n";
		}
		_o << "s\n";
	}
	else if (style == QwtCurve::Dots) {
		for (i = 0; i < x.size(); ++i) {
			x1 = (x[i]-xmin)*xscale;
			y1 = (y[i]-ymin)*yscale;
			
			_o << "n\n" << x1 << " " << y1 << " bullet\n";
		}
	}
	else if (style == QwtCurve::Histogram) {
		float x1,x2=0,y2;
		for ( i = 0;i < x.size(); ++i) {
			x1 = (x[i]-xmin)*xscale;
			y2 = (y[i]-ymin)*yscale;
			
			x2 += width;

			_o << "n\n" << x1 << " 0 m\n";
			_o << "0 " << y2 << " r\n" << width << " 0 r\n";
			_o << "0 " << -y2 << " r\nclosepath\ngsave\n0.5 setgray\nfill\n";
			_o << "grestore\n1 setlinewidth\n0 setgray\nstroke\n";
		}
	}
	else {
		for (i = 0; i < x.size(); ++i) {
			x1 = (x[i]-xmin)*xscale;
			y1 = (y[i]-ymin)*yscale;

			_o << "n\n" << x1 << " " << y1 << " bullet\n";
		}
	}
	_o << "gr\n";
}

void SimplePs::drawYRightAxis( std::string l)
{
	int i,j;
	double ymin,ymax,y_step;
	char buf[32];

	drawLine(AXISLEN,0,0,AXISLEN,1);

	ymin = _plot->axisScale(QwtPlot::yRight)->lBound();
	ymax = _plot->axisScale(QwtPlot::yRight)->hBound();
	
	y_step = (ymax-ymin)/MAJOR;

	for (i = 0; i <= MAJOR; ++i) {
		sprintf(buf, "%.2f",ymin+i*y_step);
		drawText(AXISLEN+10,i*AXISLEN/MAJOR-3,"Times-Roman",8,0,buf);
		drawTic(AXISLEN,i*AXISLEN/MAJOR,0,1,1);
	}
	for (j = 1; j < MAJOR*MINOR; ++j) 
		drawTic(AXISLEN,j*AXISLEN/(MAJOR*MINOR),0,0,1);

	// !!!hard coded part needs to be replaced by computing bounding boxes
	drawText(-50,300,"Times-Roman",12,-90,l);
}

void SimplePs::drawAxis(std::string xl, std::string yl, std::string title, bool diag)
{
	int i,j;
	double xmin,xmax,ymin,ymax,x_step,y_step;
	char buf[32];


	xmin = _plot->axisScale(QwtPlot::xBottom)->lBound();
	xmax = _plot->axisScale(QwtPlot::xBottom)->hBound();
	ymin = _plot->axisScale(QwtPlot::yLeft)->lBound();
	ymax = _plot->axisScale(QwtPlot::yLeft)->hBound();
	
	x_step = (xmax-xmin)/MAJOR;
	y_step = (ymax-ymin)/MAJOR;

	drawLine(0,0,AXISLEN,0,1);
	drawLine(0,0,0,AXISLEN,1);
	if (diag)
		drawLine(0,0,(int)(0.83*AXISLEN),(int)(0.83*AXISLEN),1);

	for (i = 0; i <= MAJOR; ++i) {
		sprintf(buf, "%.2f",xmin+i*x_step);
		drawText(i*AXISLEN/MAJOR-10,-18,"Times-Roman",8,0,buf);
		sprintf(buf, "%.2f",ymin+i*y_step);
		drawText(-40,i*AXISLEN/MAJOR-3,"Times-Roman",8,0,buf);

		drawTic(i*AXISLEN/MAJOR, 0, 1, 1,0);
		drawTic(0,i*AXISLEN/MAJOR,0,1,0);
	}
	for (j = 1; j < MAJOR*MINOR; ++j) {
		drawTic(j*AXISLEN/(MAJOR*MINOR),0,1,0,0);
		drawTic(0,j*AXISLEN/(MAJOR*MINOR),0,0,0);
	}

	//drawText(20,AXISLEN+40,"Times-Roman",20,0,title);
	drawText(0,45,"Times-Roman",12,90,yl);
	drawText(0,-40,"Times-Roman",12,0,xl);

	if (_drawStats)
		draw_stats();
}

void SimplePs::draw_stats()
{
	int y_pos = AXISLEN;
	for (Itr itr = _stats.begin(); itr != _stats.end();  ++itr) {
		drawText(AXISLEN+30,y_pos,"Times-Roman",10,0, (*itr).first);
		drawText(AXISLEN+95,y_pos,"Times-Roman",10,0,(*itr).second);
		y_pos -= 15;
	}
}

