#pragma once

#include "includes.h"
#include "MessageQueue.h"
#include "VulkanContext.h"

struct Renderer {
	VulkanContext* context;

	MessageQueue<RenderCommand> commandQueue;
	MessageQueue<RenderMessage> messageQueue;

	void init();
	bool render();
	void cleanup();

	void processCommand(RenderCommand inCmd);
	void processCommands();
};

