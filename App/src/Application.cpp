#include "Application.h"

#include "Entity.h"
#include "Core/DynamicArray.h"
#include <vector>
#include <iostream>

using namespace Composia;

void Application::Start()
{
	std::vector<int> test;
	Core::DynamicArray<int> arr;
	arr.Resize(10,2);

	std::cout << arr.At(1);

}