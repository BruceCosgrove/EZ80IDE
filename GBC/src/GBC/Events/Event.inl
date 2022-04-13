namespace gbc
{
	constexpr bool Event::IsInCategory(EventCategoryFlags category) const noexcept { return category & GetCategoryFlags(); }

	template<typename C, typename E>
	bool EventDispatcher::Dispatch(C* object, void(C::* func)(E&))
	{
		if (!event.handled && event.GetType() == E::GetStaticType())
		{
			(object->*func)(static_cast<E&>(event));
			return true;
		}
		return false;
	}

	template<typename E>
	bool EventDispatcher::Dispatch(void(*func)(E&))
	{
		if (!event.handled && event.GetType() == E::GetStaticType())
		{
			func(static_cast<E&>(event));
			return true;
		}
		return false;
	}

	template<typename C>
	bool EventDispatcher::Dispatch(C* object, void(C::* func)(Event&))
	{
		if (!event.handled)
		{
			(object->*func)(event);
			return true;
		}
		return false;
	}

	constexpr void EventDispatcher::Dispatch(void(*func)(Event&))
	{
		if (!event.handled)
			func(event);
	}
}
