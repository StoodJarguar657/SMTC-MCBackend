import { MessageFlags, SlashCommandBuilder } from "discord.js"

export default {
    data: new SlashCommandBuilder()
        .setName("get-public-ip")
        .setDescription("Gets the public ip address of the server. (If allowed)"),

    permissionLevel: 1,

    async init() {},

    /**
     * @param {import("discord.js").ChatInputCommandInteraction} interaction 
     * @param {import("serverInfo").ServerInfo} serverInfo 
     * @param {number} permissionLevel
     */
    async execute(interaction, serverInfo, permissionLevel) {
        if(serverInfo.exposePublicIp === 0 && permissionLevel < 2)
            return await interaction.reply({ content: "You are not allowed to see the public ip of the server!", flags: MessageFlags.Ephemeral })

        const response = await fetch("https://api.ipify.org/?format=text")
        const ip = await response.text()

        await interaction.reply({ content: `**Public Address:** \`${ip}:${serverInfo.publicIpPort}\``})
    }
}