namespace gbc
{
	template<typename T>
	T* EZ80IDELayer::AddPanel(const std::string& name)
	{
		T* panel = new T(this, name);
		panels.Push(panel);
		return panel;
	}
}
