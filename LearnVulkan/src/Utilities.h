#pragma once

// Indices (location) of Queue Families (if they exist at all)
struct QueueFamilyIndices
{
	int graphicsFamily = -1;			// Location of Graphics Queue Family

	// Check if Queue Families Are Valid.
	bool IsValid()
	{
		return graphicsFamily >= 0;
	}
};