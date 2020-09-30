#pragma once

class InputClass
{
public:
	InputClass();
	InputClass(const InputClass&);
	~InputClass();

	void Initialize();

	void KeyDown(UINT);
	void KeyUp(UINT);

	bool IsKeyDown(UINT);

private:
	bool m_keys[256];
};