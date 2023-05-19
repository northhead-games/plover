#pragma once

#include "includes.h"
#include "MessageQueue.h"
#include "VulkanContext.h"
#include "AssetLoader.h"

struct Renderer {
	VulkanContext* context;

	MessageQueue<RenderCommand> commandQueue;
	MessageQueue<RenderMessage> messageQueue;
	AssetLoader loader;

	void init();
	bool render();
	void cleanup();

	void processCommand(RenderCommand inCmd);
	void processCommands();
};

