#pragma once
#include <cmath>
#include <vector>

class Graph {
	
public:
	std::vector<float>vertices;
	std::vector<unsigned int>indices;

	Graph()
	{
		getVertices();
	}

	float function(float x, float y)
	{
		return sqrt(x) + sqrt(y);
	}

	void getVertices()
	{
		float s, t;
		for (float x = 0.0f; x < 2.0f; x += 0.1f)
		{
			for (float y = 0.00f; y < 2.0f; y += 0.1f)
			{
				float z = function(x, y);
				vertices.push_back(x);
				vertices.push_back(y);
				vertices.push_back(-z);
			}
		}
		
		int x = (int)round(abs(2) / 0.1);
		int y = (int)round(abs(2) / 0.1);

		int maxX = x;
		int maxY = y;

		int index = 0;
		bool jump = false;

		for (int i = 0; i < maxX - 1; i++)
		{
			for (int j = 0; j < maxX + maxY - 1; j++)
			{
				indices.push_back(index);
				jump = !jump;
				if (jump)
				{
					index += abs(y);
					continue;
				}
				index -= abs(y - 1);
			}
			jump = !jump;
			y = -y;
		}
		indices.push_back(index);
	}
};