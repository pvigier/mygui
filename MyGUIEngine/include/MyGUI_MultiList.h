/*!
	@file
	@author		Albert Semenov
	@date		04/2008
	@module
*/
#ifndef __MYGUI_MULTI_LIST_H__
#define __MYGUI_MULTI_LIST_H__

#include "MyGUI_Prerequest.h"
#include "MyGUI_Widget.h"
#include "MyGUI_List.h"
#include "MyGUI_Any.h"
#include "MyGUI_BiIndexBase.h"

namespace MyGUI
{

	typedef delegates::CDelegate5<WidgetPtr, size_t, const Ogre::UTFString &, const Ogre::UTFString &, bool &> EventInfo_WidgetIntUTFStringUTFStringBool;

	class MYGUI_EXPORT MultiList : public Widget, public BiIndexBase
	{
		// для вызова закрытого конструктора
		friend class factory::BaseWidgetFactory<MultiList>;

		MYGUI_RTTI_CHILD_HEADER( MultiList, Widget );

	public:
		//------------------------------------------------------------------------------//
		// Methods for work with columns (RU:методы для работы со столбцами)
		//------------------------------------------------------------------------------//
		// манипуляции айтемами

		//! Get number of columns
		size_t getColumnCount() { return mVectorColumnInfo.size(); }

		/** Insert new column
			@param _column New column will be inserted before _column
			@param _name Name of new column
			@param _width Width of new column
		*/
		void insertColumnAt(size_t _column, const Ogre::UTFString & _name, int _width, Any _data = Any::Null);

		/** Add new column at last position
			@param _width Width of new column
			@param _name Name of new column
		*/
		void addColumn(const Ogre::UTFString & _name, int _width, Any _data = Any::Null) { insertColumnAt(ITEM_NONE, _name, _width, _data); }

		/** Delete column */
		void removeColumnAt(size_t _column);

		/** Delete all columns */
		void removeAllColumns();


		//------------------------------------------------------------------------------//
		// манипуляции отображением

		/** Set column name
			@param _column Index of column
			@param _name New name of column
		*/
		void setColumnNameAt(size_t _column, const Ogre::UTFString & _name);

		/** Set column width
			@param _column Index of column
			@param _name New width of column
		*/
		void setColumnWidthAt(size_t _column, int _width);

		/** Get _column name */
		const Ogre::UTFString & getColumnNameAt(size_t _column);

		/** Get _column width */
		int getColumnWidthAt(size_t _column);

		/** Sort multilist by column */
		void sortByColumn(size_t _column, bool _backward = false);

		//------------------------------------------------------------------------------//
		// манипуляции данными

		//! Replace an item data at a specified position
		void setColumnDataAt(size_t _index, Any _data)
		{
			MYGUI_ASSERT_RANGE(_index, mVectorColumnInfo.size(), "MultiList::setColumnDataAt");
			mVectorColumnInfo[_index].data = _data;
		}

		//! Clear an item data at a specified position
		void clearColumnDataAt(size_t _index) { setColumnDataAt(_index, Any::Null); }

		//! Get item data from specified position
		template <typename ValueType>
		ValueType * getColumnDataAt(size_t _index, bool _throw = true)
		{
			MYGUI_ASSERT_RANGE(_index, mVectorColumnInfo.size(), "MultiList::getItemDataAt");
			return mVectorColumnInfo[_index].data.castType<ValueType>(_throw);
		}

		//------------------------------------------------------------------------------//
		// Methods for work with lines (RU:методы для работы со строками)
		/** @note
      	All indexes used here is indexes of unsorted Multilist. Even if you sorted
			it - all items indexes will be same as before sort.*/

		//------------------------------------------------------------------------------//
		// манипуляции айтемами

		/** Get number of items (lines) */
		size_t getItemCount();

		/** Insert new item before _index line */
		void insertItemAt(size_t _index, const Ogre::UTFString & _name, Any _data = Any::Null);

		/** Add new item at the end */
		void addItem(const Ogre::UTFString & _name, Any _data = Any::Null) { insertItemAt(ITEM_NONE, _name, _data); }

		void removeItemAt(size_t _index);

		/** Delete all items */
		void removeAllItems();

		void swapItemsAt(size_t _index1, size_t _index2);


		//------------------------------------------------------------------------------//
		// манипуляции отображением

		/** Set item string */
		void setItemNameAt(size_t _index, const Ogre::UTFString & _name) { setSubItemNameAt(0, _index, _name); }

		const Ogre::UTFString & getItemNameAt(size_t _index) { return getSubItemNameAt(0, _index); }


		//------------------------------------------------------------------------------//
		// манипуляции выделениями

		/** Get index of selected item (ITEM_NONE if none selected) */
		size_t getIndexSelected() { return mItemSelected; }

		/** Select specified _index */
		void setIndexSelected(size_t _index);

		/** Clear item selection */
		void clearIndexSelected() { setIndexSelected(ITEM_NONE); }


		MYGUI_OBSOLETE("use : size_t MultiList::getIndexSelected()")
		size_t getItemIndexSelected() { return getIndexSelected(); }
		MYGUI_OBSOLETE("use : void MultiList::setIndexSelected(size_t _index)")
		void setItemSelectedAt(size_t _index) { setIndexSelected(_index); }
		MYGUI_OBSOLETE("use : void MultiList::clearIndexSelected()")
		void clearItemSelected() { clearIndexSelected(); }

		//------------------------------------------------------------------------------//
		// манипуляции данными

		//! Replace an item data at a specified position
		void setItemDataAt(size_t _index, Any _data) { setSubItemDataAt(0, _index, _data); }

		//! Clear an item data at a specified position
		void clearItemDataAt(size_t _index) { setItemDataAt(_index, Any::Null); }

		//! Get item data from specified position
		template <typename ValueType>
		ValueType * getItemDataAt(size_t _index, bool _throw = true)
		{
			return getSubItemDataAt<ValueType>(0, _index, _throw);
		}


		//------------------------------------------------------------------------------//
		// Methods for work with sub lines (RU:методы для работы со саб строками)
		//------------------------------------------------------------------------------//
		// манипуляции данными

		/** Set sub item
			@param _column Index of column
			@param _index Index of line
			@param _name New sub item value
		*/
		void setSubItemNameAt(size_t _column, size_t _index, const Ogre::UTFString & _name);

		/** Get sub item name*/
		const Ogre::UTFString & getSubItemNameAt(size_t _column, size_t _index);

		/** Search item in specified _column, returns index of the first occurrence in column or ITEM_NONE if item not found */
		size_t findSubItemWith(size_t _column, const Ogre::UTFString & _name);

		//------------------------------------------------------------------------------//
		// манипуляции данными

		//! Replace an item data at a specified position
		void setSubItemDataAt(size_t _column, size_t _index, Any _data)
		{
			MYGUI_ASSERT_RANGE(_column, mVectorColumnInfo.size(), "MultiList::setSubItemDataAt");
			MYGUI_ASSERT_RANGE(_index, mVectorColumnInfo.begin()->list->getItemCount(), "MultiList::setSubItemDataAt");

			size_t index = BiIndexBase::convertToBack(_index);
			mVectorColumnInfo[_column].list->setItemDataAt(index, _data);
		}

		//! Clear an item data at a specified position
		void clearSubItemDataAt(size_t _column, size_t _index) { setSubItemDataAt(_column, _index, Any::Null); }

		//! Get item data from specified position
		template <typename ValueType>
		ValueType * getSubItemDataAt(size_t _column, size_t _index, bool _throw = true)
		{
			MYGUI_ASSERT_RANGE(_column, mVectorColumnInfo.size(), "MultiList::getSubItemDataAt");
			MYGUI_ASSERT_RANGE(_index, mVectorColumnInfo.begin()->list->getItemCount(), "MultiList::getSubItemDataAt");

			size_t index = BiIndexBase::convertToBack(_index);
			return mVectorColumnInfo[_column].list->getItemDataAt<ValueType>(index, _throw);
		}


		// #ifdef MYGUI_USING_OBSOLETE

		MYGUI_OBSOLETE("use : size_t MultiList::findSubItemWith(size_t _column, const Ogre::UTFString & _name)")
		size_t findItem(size_t _column, const Ogre::UTFString & _name) { return findSubItemWith(_column, _name); }

		MYGUI_OBSOLETE("use : const Ogre::UTFString & MultiList::getSubItemNameAt(size_t _column, size_t _index)")
		const Ogre::UTFString & getSubItem(size_t _column, size_t _index) { return getSubItemNameAt(_column, _index); }

		MYGUI_OBSOLETE("use : void MultiList::setSubItemNameAt(size_t _column, size_t _index, const Ogre::UTFString & _name)")
		void setSubItem(size_t _column, size_t _index, const Ogre::UTFString & _name) { setSubItemNameAt(_column, _index, _name); }

		MYGUI_OBSOLETE("use : void MultiList::removeColumnAt(size_t _column)")
		void deleteColumn(size_t _column) { removeColumnAt(_column); }

		MYGUI_OBSOLETE("use : void MultiList::removeAllColumns()")
		void deleteAllColumns() { removeAllColumns(); }

		MYGUI_OBSOLETE("use : int MultiList::getColumnWidthAt(size_t _column)")
		int getColumnWidth(size_t _column) { return getColumnWidthAt(_column); }

		MYGUI_OBSOLETE("use : const Ogre::UTFString & MultiList::getColumnNameAt(size_t _column)")
		const Ogre::UTFString & getColumnName(size_t _column) { return getColumnNameAt(_column); }

		MYGUI_OBSOLETE("use : void MultiList::setColumnWidthAt(size_t _column, int _width)")
		void setColumnWidth(size_t _column, int _width) { setColumnWidthAt(_column, _width); }

		MYGUI_OBSOLETE("use : void MultiList::addColumn(const Ogre::UTFString & _name, int _width, Any _data)")
		void addColumn(int _width, const Ogre::UTFString & _name) { addColumn(_name, _width); }

		MYGUI_OBSOLETE("use : void MultiList::setColumnNameAt(size_t _column, const Ogre::UTFString & _name)")
		void setColumnName(size_t _column, const Ogre::UTFString & _name) { setColumnNameAt(_column, _name); }

		MYGUI_OBSOLETE("use : void MultiList::insertColumnAt(size_t _column, const Ogre::UTFString & _name, int _width, Any _data)")
		void insertColumn(size_t _column, int _width, const Ogre::UTFString & _name) { insertColumnAt(_column, _name, _width); }

		MYGUI_OBSOLETE("use : size_t MultiList::getIndexSelected()")
		size_t getItemSelect() { return getIndexSelected(); }

		MYGUI_OBSOLETE("use : void MultiList::clearIndexSelected()")
		void resetItemSelect() { clearIndexSelected(); }

		MYGUI_OBSOLETE("use : void MultiList::setIndexSelected(size_t _index)")
		void setItemSelect(size_t _index) { setIndexSelected(_index); }

		MYGUI_OBSOLETE("use : void MultiList::insertItemAt(size_t _index, const Ogre::UTFString & _name, Any _data)")
		void insertItem(size_t _index, const Ogre::UTFString & _name) { insertItemAt(_index, _name); }

		MYGUI_OBSOLETE("use : void MultiList::setItemNameAt(size_t _index, const Ogre::UTFString & _name)")
		void setItem(size_t _index, const Ogre::UTFString & _name) { setItemNameAt(_index, _name); }

		MYGUI_OBSOLETE("use : const Ogre::UTFString & MultiList::getItemNameAt(size_t _index)")
		const Ogre::UTFString & getItem(size_t _index) { return getItemNameAt(_index); }

		MYGUI_OBSOLETE("use : void MultiList::removeItemAt(size_t _index)")
		void deleteItem(size_t _index) { removeItemAt(_index); }

		MYGUI_OBSOLETE("use : void MultiList::removeAllItems()")
		void deleteAllItems() { removeAllItems(); }

		// #endif // MYGUI_USING_OBSOLETE


		/** Event : Enter pressed or double click.\n
			signature : void method(MyGUI::WidgetPtr _sender, size_t _index)\n
			@param _sender widget that called this event
			@param _index of selected item
		*/
		EventInfo_WidgetSizeT eventListSelectAccept;

		/** Event : Selected item position changed.\n
			signature : void method(MyGUI::WidgetPtr _sender, size_t _index)\n
			@param _sender widget that called this event
			@param _index of new item
		*/
		EventInfo_WidgetSizeT eventListChangePosition;

		/** Event : Less than operator for sort multilist by columns.\n
			signature : void method(MyGUI::WidgetPtr _sender, size_t _column, const Ogre::UTFString & _firstItem, const Ogre::UTFString & _secondItem, bool & _less)\n
			@param _sender widget that called this event
			@param _column Index of column
			@param _firstItem, _secondItem Strings for compare
			@param _less Comparsion result (write your value here)
		*/
		EventInfo_WidgetIntUTFStringUTFStringBool operatorLess;

	protected:
		MultiList(WidgetStyle _style, const IntCoord& _coord, Align _align, const WidgetSkinInfoPtr _info, WidgetPtr _parent, ICroppedRectangle * _croppedParent, IWidgetCreator * _creator, const std::string & _name);
		virtual ~MultiList();

		void baseChangeWidgetSkin(WidgetSkinInfoPtr _info);

		void notifyListChangePosition(MyGUI::WidgetPtr _widget, size_t _position);
		void notifyListChangeFocus(MyGUI::WidgetPtr _widget, size_t _position);
		void notifyListChangeScrollPosition(MyGUI::WidgetPtr _widget, size_t _position);
		void notifyButtonClick(MyGUI::WidgetPtr _widget);
		void notifyListSelectAccept(MyGUI::WidgetPtr _widget, size_t _position);

		void updateColumns();
		void redrawButtons();
		void updateOnlyEmpty();

		bool compare(ListPtr _list, size_t _left, size_t _right);
		void sortList();
		void flipList();

		void setDirtySort();

		WidgetPtr getSeparator(size_t _index);

		void setButtonImageIndex(ButtonPtr _button, size_t _index);

		void updateBackSelected(size_t _index);

	private:
		void initialiseWidgetSkin(WidgetSkinInfoPtr _info);
		void shutdownWidgetSkin();

		void frameEntered(float _frame);

	private:
		struct ColumnInfo
		{
			ListPtr list;
			ButtonPtr button;
			int width;
			Ogre::UTFString name;
			Any data;
		};

		typedef std::vector<ColumnInfo> VectorColumnInfo;

		enum ImageSort
		{
			SORT_NONE,
			SORT_UP,
			SORT_DOWN
		};

	private:
		int mHeightButton;
		int mWidthBar;
		std::string mSkinButton, mSkinList;
		ButtonPtr mButtonMain;

		VectorColumnInfo mVectorColumnInfo;

		VectorWidgetPtr mSeparators;

		size_t mLastMouseFocusIndex;

		bool mSortUp;
		size_t mSortColumnIndex;

		bool mIsDirtySort;

		int mWidthSeparator;
		std::string mSkinSeparator;
		int mOffsetButtonSeparator;

		size_t mItemSelected;

	};

} // namespace MyGUI

#endif // __MYGUI_MULTI_LIST_H__
