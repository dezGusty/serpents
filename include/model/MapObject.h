#pragma once

//    This file is part of Gusty's Serpents
//    Copyright (C) 2009  Augustin Preda (thegusty999@gmail.com)
//
//    Gusty's Serpents is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//    Base class map objects.

#include "utils/GGeneralUtils.h"
#include "GRenderable.h"

namespace Serpents
{
	class MapObject;

	///
	///	Class that defines a simple map object. This could be a bonus, a piece of a snake...
	///
	class MapObject
	{
	public:

		///
		///	definition for supported pivot point types.
		///
		typedef enum 
		{
			TopLeftCornerPivot,
			CenterPivot,
			NumPivots
		}PivotPoint;

		///
		///	definitions of supported basic geometric shapes for the objects.
		///
		typedef enum
		{
			ObjectShapeRectangular,
			ObjectShapeCircular,
			ObjectShapeSquare,
			NumShapes
		}ObjectShape;


		///
		///	Default constructor.
		///

		MapObject();

		///
		///	Initialize the map object within a rectangle space
		///
		MapObject( GUtils::DRECT initialRect );

		///
		///	Destructor. No pointers need to be cleared.
		///
		virtual ~MapObject(void);

		///
		///	Set the bounds for the object.
		///

		virtual void setBounds( GUtils::DRECT someRect ){ oldRect = rect; rect = someRect; }

		///
		///	Get the bounds of the object.
		///

		virtual GUtils::DRECT getBounds() const { return rect; }

		///
		///	Get the location of the shape (this will be actually the position of the pivot point).
		///
		virtual GUtils::DPOINT getLocation() const;


		///
		///	Get the previous bounds of the shape.
		///

		virtual GUtils::DRECT getPreviousBounds() const { return oldRect; }

		///
		///	Get the size of the shape (width & height).
		///

		virtual GUtils::DPOINT getSize() const;

		///
		///	Get the shape type for the object.
		///

		virtual ObjectShape getShape(){ return shape; }

		///
		///	Get the size from the center of the object to its border.
		///	NOTE: this is currently usable mostly for circles. For all other shapes, the result will only
		/// give aproximate values.
		///

		GUtils::DFLOAT getSizeFromCenter() const;

		///
		///	Set the pivot point to use.
		///

		virtual void setPivot( PivotPoint point );

		///
		///	Set the shape to use for the current object when calculating collisions.
		///

		virtual void setShape( ObjectShape newShape ){ shape = newShape; }

		///
		///	Set the size (changes bounds) relative to the pivot point.
		///

		virtual void setSize( GUtils::DPOINT newSize ){ setSize(newSize.x, newSize.y); }

		///
		///	Set the size (changes bounds) relative to the pivot point.
		///
		virtual void setSize( GUtils::DFLOAT x, GUtils::DFLOAT y );

		///
		///	Set the location to a new point relative to the pivot point.
		///

		virtual void setLocation( GUtils::DPOINT newLocation ){ setLocation(newLocation.x, newLocation.y); }

		///
		///	Set the location to a new point relative to the pivot point.
		///
		virtual void setLocation( GUtils::DFLOAT x, GUtils::DFLOAT y );


		///
		///	Check intersection (shape overlapping) with another object
		///	@param someObject Object with which to compare
		/// @param checkBorderCollision Specify wether the 2 objects are considered colliding if they 
		///	have a line or a point as the result of the intersection.
		///

		virtual bool intersectsWith( const MapObject * someObject, bool checkBorderCollision = false );

		///
		///	Check intersection (shape overlapping) with another object
		///	@param rhs Object with which to compare
		/// @param checkBorderCollision Specify wether the 2 objects are considered colliding if they 
		///	have a line or a point as the result of the intersection.
		///	@obsolete!!!
		///

		virtual bool intersectsWithArea( GUtils::DRECT area, bool checkBorderCollision = false, ObjectShape oshape = ObjectShapeRectangular );

		///
		///	Common init function; called from root constructors.
		///
		void init(GUtils::DFLOAT, GUtils::DFLOAT, GUtils::DFLOAT, GUtils::DFLOAT);

	protected:
	private:
		//	the current bounds of the object
		GUtils::DRECT rect;

		//	the previous bounds of the object. Note that this only changes with the movement of the map object, 
		//	not resizing or pivot point changing.
		GUtils::DRECT oldRect;

		PivotPoint pivot;	//	Pivot point - mostly used for resizing
		ObjectShape shape;	//	Basic shape used for collision detection
	};


}