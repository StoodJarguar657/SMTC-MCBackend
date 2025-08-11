import { configDotenv } from "dotenv";
configDotenv()

import commandManager from "./commandManager.js";

import { ActivityType, Client, Events, GatewayIntentBits } from "discord.js";
import serverManager from "./serverManager.js";
import sendMessage from "./sendMessage.js";

const client = new Client({ intents: [GatewayIntentBits.Guilds, GatewayIntentBits.GuildMessages, GatewayIntentBits.MessageContent]})

commandManager.init()

client.once(Events.ClientReady, (readyClient) => {
	console.log(`Ready! Logged in as ${readyClient.user.tag}`);

    const statuses = [
		"Spying on MC servers...",
		"Questioning why theres a lag machine...",
		"Who murdered my dog?",
		"This backend sucks fr fr"
	];

	let index = 0;
	setInterval(() => {
		client.user.setActivity(statuses[index], { type: ActivityType.Custom});
		index = (index + 1) % statuses.length;
	}, 5000);
});

client.on(Events.InteractionCreate, async (interaction) => {
    if(interaction.isChatInputCommand()) {
        await commandManager.runCommand(interaction)
        return
    }
})

client.on(Events.MessageCreate, async (message) => {
    if(message.member.user.bot) return

    // @ts-ignore
    const serverInfo = await serverManager.getServerInfo(message)
    if(!serverInfo)
        return

    await sendMessage.sendMessage(serverInfo, message.member.nickname || message.member.displayName || message.member.user.username, message.member.id, message.content)
})

client.login(process.env.TOKEN);