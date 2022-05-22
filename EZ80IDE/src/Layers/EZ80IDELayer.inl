namespace ide
{
	template<typename T>
	T* EZ80IDELayer::AddPanel(const std::string& name)
	{
		T* panel = new T(this, name);
		m_Panels.Push(panel);
		return panel;
	}
}
