import { MessageFlags, SlashCommandBuilder } from "discord.js"
import { fileURLToPath, pathToFileURL } from 'url';
import path from 'path';
import fs from "fs/promises"

export default {
    data: new SlashCommandBuilder()
        .setName("admin")
        .setDescription("Admin Commands"),
    permissionLevel: 2,

    /**
     * @type {import("subcommand").SubCommand[]}
     */
    subCommands: [],

    async init() {
        const filePath = fileURLToPath(import.meta.url);
        const basename = path.basename(filePath).replace(".js", "")
        const commandsDir = path.join(path.dirname(filePath), basename);

        const commands = await fs.readdir(commandsDir);

        for (const commandFile of commands) {
            if (commandFile === path.basename(filePath)) continue;

            const fullPath = path.join(commandsDir, commandFile);
            const commandUrl = pathToFileURL(fullPath).href;

            console.log(`Registering subcommand \"${commandFile.replace(".js", "")}\"`)
            
            /**
             * @type {import("subcommand").SubCommand}
             */
            const cmd = (await import(commandUrl)).default;
            await cmd.init()

            this.data.addSubcommand(cmd.data)
            this.subCommands.push(cmd)
        }
    },

    /**
     * @param {import("discord.js").ChatInputCommandInteraction} interaction 
     * @param {import("serverInfo").ServerInfo} serverInfo 
     * @param {number} permissionLevel
     */
    async execute(interaction, serverInfo, permissionLevel) {
        const subcommandName = interaction.options.getSubcommand();
        const subcommand = this.subCommands.find(cmd => cmd.data.name === subcommandName);

        if(subcommand.permissionLevel > permissionLevel)
            return await interaction.reply({ content: "You do not have permission to run this command!", flags: MessageFlags.Ephemeral})

        if(subcommand)
           return await subcommand.execute(interaction, serverInfo);
        
        return interaction.reply({ content: `Unknown subcommand: ${subcommandName}`, flags: MessageFlags.Ephemeral });
    }
}