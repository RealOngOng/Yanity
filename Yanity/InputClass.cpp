#include "stdafx.h"
#include "InputClass.h" 

InputClass::InputClass()
{

}
InputClass::InputClass(const InputClass& other)
{

}
InputClass::~InputClass()
{

}
void InputClass::Initialize()
{
	std::fill(m_keys, m_keys + 256, false);
}
void InputClass::KeyDown(UINT input)
{
	m_keys[input] = true;
}
void InputClass::KeyUp(UINT input)
{
	m_keys[input] = false;
}
bool InputClass::IsKeyDown(UINT input)
{
	return m_keys[input];
}