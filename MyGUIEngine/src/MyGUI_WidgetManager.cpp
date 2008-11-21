/*!
	@file
	@author		Albert Semenov
	@date		11/2007
	@module
*/
#include "MyGUI_Gui.h"
#include "MyGUI_WidgetManager.h"
#include "MyGUI_LayerManager.h"
#include "MyGUI_Widget.h"
#include "MyGUI_IWidgetCreator.h"

#include "MyGUI_WidgetFactory.h"
#include "MyGUI_ButtonFactory.h"
#include "MyGUI_WindowFactory.h"
#include "MyGUI_ListFactory.h"
#include "MyGUI_VScrollFactory.h"
#include "MyGUI_HScrollFactory.h"
#include "MyGUI_EditFactory.h"
#include "MyGUI_ComboBoxFactory.h"
#include "MyGUI_StaticTextFactory.h"
#include "MyGUI_TabFactory.h"
#include "MyGUI_TabItemFactory.h"
#include "MyGUI_ProgressFactory.h"
#include "MyGUI_ItemBoxFactory.h"
#include "MyGUI_MultiListFactory.h"
#include "MyGUI_StaticImageFactory.h"
#include "MyGUI_MessageFactory.h"
#include "MyGUI_RenderBoxFactory.h"
#include "MyGUI_PopupMenuFactory.h"
#include "MyGUI_PopupMenuItemFactory.h"
#include "MyGUI_MenuBarFactory.h"
#include "MyGUI_ScrollViewFactory.h"
#include "MyGUI_DDContainerFactory.h"
#include "MyGUI_GridCtrlFactory.h"

namespace MyGUI
{

	INSTANCE_IMPLEMENT(WidgetManager);

	void WidgetManager::initialise()
	{
		MYGUI_ASSERT(false == mIsInitialise, INSTANCE_TYPE_NAME << " initialised twice");
		MYGUI_LOG(Info, "* Initialise: " << INSTANCE_TYPE_NAME);

		registerUnlinker(this);

		// создаем фабрики виджетов
		mIntegratedFactoryList.insert(new factory::WidgetFactory());
		mIntegratedFactoryList.insert(new factory::ButtonFactory());
		mIntegratedFactoryList.insert(new factory::WindowFactory());
		mIntegratedFactoryList.insert(new factory::ListFactory());
		mIntegratedFactoryList.insert(new factory::HScrollFactory());
		mIntegratedFactoryList.insert(new factory::VScrollFactory());
		mIntegratedFactoryList.insert(new factory::EditFactory());
		mIntegratedFactoryList.insert(new factory::ComboBoxFactory());
		mIntegratedFactoryList.insert(new factory::StaticTextFactory());
		mIntegratedFactoryList.insert(new factory::TabFactory());
		mIntegratedFactoryList.insert(new factory::TabItemFactory());
		mIntegratedFactoryList.insert(new factory::SheetFactory()); // OBSOLETE
		mIntegratedFactoryList.insert(new factory::ProgressFactory());
		mIntegratedFactoryList.insert(new factory::ItemBoxFactory());
		mIntegratedFactoryList.insert(new factory::MultiListFactory());
		mIntegratedFactoryList.insert(new factory::StaticImageFactory());
		mIntegratedFactoryList.insert(new factory::MessageFactory());
		mIntegratedFactoryList.insert(new factory::RenderBoxFactory());
		mIntegratedFactoryList.insert(new factory::PopupMenuFactory());
		mIntegratedFactoryList.insert(new factory::PopupMenuItemFactory());
		mIntegratedFactoryList.insert(new factory::MenuBarFactory());
		mIntegratedFactoryList.insert(new factory::ScrollViewFactory());
		mIntegratedFactoryList.insert(new factory::DDContainerFactory());
		mIntegratedFactoryList.insert(new factory::GridCtrlFactory());

		MYGUI_LOG(Info, INSTANCE_TYPE_NAME << " successfully initialized");
		mIsInitialise = true;
	}

	void WidgetManager::shutdown()
	{
		if (false == mIsInitialise) return;
		MYGUI_LOG(Info, "* Shutdown: " << INSTANCE_TYPE_NAME);

		unregisterUnlinker(this);

		mFactoryList.clear();
		mDelegates.clear();
		mVectorIUnlinkWidget.clear();

		for (SetWidgetFactory::iterator iter = mIntegratedFactoryList.begin(); iter != mIntegratedFactoryList.end(); ++iter) delete*iter;
		mIntegratedFactoryList.clear();

		MYGUI_LOG(Info, INSTANCE_TYPE_NAME << " successfully shutdown");
		mIsInitialise = false;
	}

	void WidgetManager::registerFactory(IWidgetFactory * _factory)
	{
		mFactoryList.insert(_factory);
		MYGUI_LOG(Info, "* Register widget factory '" << _factory->getTypeName() << "'");
	}

	void WidgetManager::unregisterFactory(IWidgetFactory * _factory)
	{
		SetWidgetFactory::iterator iter = mFactoryList.find(_factory);
		if (iter != mFactoryList.end()) mFactoryList.erase(iter);
		MYGUI_LOG(Info, "* Unregister widget factory '" << _factory->getTypeName() << "'");
	}

	WidgetPtr WidgetManager::createWidget(const Ogre::String & _type, const Ogre::String & _skin, const IntCoord& _coord, Align _align, ICroppedRectangle * _parent, IWidgetCreator * _creator, const Ogre::String & _name)
	{
		std::string name;
		if (false == _name.empty()) {
			MapWidgetPtr::iterator iter = mWidgets.find(_name);
			MYGUI_ASSERT(iter == mWidgets.end(), "widget with name '" << _name << "' already exist");
			name = _name;
		} else {
			static long num = 0;
			name = utility::toString(num++, "_", _type);
		}

		for (SetWidgetFactory::iterator factory = mFactoryList.begin(); factory != mFactoryList.end(); factory++) {
			if ( (*factory)->getTypeName() == _type) {
				WidgetPtr widget = (*factory)->createWidget(_skin, _coord, _align, _parent, _creator, name);
				mWidgets[name] = widget;
				return widget;
			}
		}
		MYGUI_EXCEPT("factory '" << _type << "' not found");
		return null;
	}

	WidgetPtr WidgetManager::findWidgetT(const Ogre::String & _name, bool _throw)
	{
		MapWidgetPtr::iterator iter = mWidgets.find(_name);
		if (iter == mWidgets.end()){
			if (_throw){
				MYGUI_ASSERT(!_throw, "Widget '" << _name << "' not found");
			}else{
				MYGUI_LOG(Error, "Widget '" << _name << "' not found");
				return null;
			}
		}
		return iter->second;
	}

	void WidgetManager::_unlinkWidget(WidgetPtr _widget)
	{
		if (_widget == null) return;
		MapWidgetPtr::iterator iter = mWidgets.find(_widget->getName());
		if (iter != mWidgets.end()) mWidgets.erase(iter);
	}

	ParseDelegate & WidgetManager::registerDelegate(const Ogre::String & _key)
	{
		MapDelegate::iterator iter = mDelegates.find(_key);
		MYGUI_ASSERT(iter == mDelegates.end(), "delegate with name '" << _key << "' already exist");
		return (mDelegates[_key] = ParseDelegate());
	}

	void WidgetManager::unregisterDelegate(const Ogre::String & _key)
	{
		MapDelegate::iterator iter = mDelegates.find(_key);
		if (iter != mDelegates.end()) mDelegates.erase(iter);
	}

	void WidgetManager::parse(WidgetPtr _widget, const Ogre::String &_key, const Ogre::String &_value)
	{
		MapDelegate::iterator iter = mDelegates.find(_key);
		if (iter == mDelegates.end()) {
			MYGUI_LOG(Error, "Unknown key '" << _key << "' with value '" << _value << "'");
			return;
		}
		iter->second(_widget, _key, _value);
	}

	void WidgetManager::destroyWidget(WidgetPtr _widget)
	{
		// иначе возможен бесконечный цикл
		MYGUI_ASSERT(_widget != null, "widget is deleted");

		// делегирует удаление отцу виджета
		IWidgetCreator * creator = _widget->_getIWidgetCreator();
		creator->_destroyChildWidget(_widget);

	}

	void WidgetManager::destroyWidgets(VectorWidgetPtr & _widgets)
	{
		for (VectorWidgetPtr::iterator iter = _widgets.begin(); iter != _widgets.end(); ++iter)
		{
			destroyWidget(*iter);
		}
	}

	void WidgetManager::destroyWidgets(EnumeratorWidgetPtr & _widgets)
	{
		VectorWidgetPtr widgets;
		while (_widgets.next()) {
			widgets.push_back(_widgets.current());
		};
		destroyWidgets(widgets);
	}

	void WidgetManager::destroyAllWidget()
	{
		//Gui::getInstance()._destroyAllChildWidget();
	}

	void WidgetManager::registerUnlinker(IUnlinkWidget * _unlink)
	{
		unregisterUnlinker(_unlink);
		mVectorIUnlinkWidget.push_back(_unlink);
	}

	void WidgetManager::unregisterUnlinker(IUnlinkWidget * _unlink)
	{
		for (size_t pos=0; pos<mVectorIUnlinkWidget.size(); pos++) {
			if (mVectorIUnlinkWidget[pos] == _unlink) {
				mVectorIUnlinkWidget[pos] = mVectorIUnlinkWidget[mVectorIUnlinkWidget.size()-1];
				mVectorIUnlinkWidget.pop_back();
				return;
			}
		}
	}

	void WidgetManager::unlinkFromUnlinkers(WidgetPtr _widget)
	{
		for (size_t pos=0; pos<mVectorIUnlinkWidget.size(); pos++) {
			mVectorIUnlinkWidget[pos]->_unlinkWidget(_widget);
		}
	}

	IntCoord WidgetManager::convertRelativeToInt(const FloatCoord& _coord, WidgetPtr _parent)
	{
		if (null == _parent) {
			const IntSize & size = Gui::getInstance().getViewSize();
			return IntCoord(int(_coord.left * size.width), int(_coord.top * size.height), int(_coord.width * size.width), int(_coord.height * size.height));
		}
		const IntCoord& coord = _parent->getClientCoord();
		return IntCoord(int(_coord.left * coord.width), int(_coord.top * coord.height), int(_coord.width * coord.width), int(_coord.height * coord.height));
	}

	FloatCoord WidgetManager::convertIntToRelative(const IntCoord& _coord, WidgetPtr _parent)
	{
		if (null == _parent) {
			const IntSize & size = Gui::getInstance().getViewSize();
			return FloatCoord(_coord.left / (float)size.width, _coord.top / (float)size.height, _coord.width / (float)size.width, _coord.height / (float)size.height);
		}
		const IntCoord& coord = _parent->getClientCoord();
		return FloatCoord(1.*_coord.left / coord.width, 1.*_coord.top / coord.height, 1.*_coord.width / coord.width, 1.*_coord.height / coord.height);
	}

} // namespace MyGUI
