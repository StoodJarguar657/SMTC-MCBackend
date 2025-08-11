import fs from "fs/promises"
import database from "./database.js"
import serverManager from "./serverManager.js"
import { MessageFlags } from "discord.js"

export default {
    /**
     * @type {import("command").Command[]}
     */
    commands: [],

    async init() {
        console.log("CommandManager.init()")

        const commandsDir = (await fs.readdir("commands")).filter(item => item.endsWith(".js"))
        for(const file of commandsDir) {
            const command = (await import(`../commands/${file}`)).default

            console.log(`Registering command \"${file.replace(".js", "")}\"`)
            await command.init()

            this.commands.push(command)
        }
    },

    /**
     * @param {import("discord.js").ChatInputCommandInteraction} interaction 
     */
    async runCommand(interaction) {
        console.log(`CommandManager.runCommand(<Interaction ${interaction.id}>) -> (Command Name: "${interaction.commandName}", Username: @${interaction.user.username}, UserId: ${interaction.user.id})`)
        
        database.get("SELECT permLevel FROM users WHERE userId = ?", [interaction.user.id], async (err, row) => {
            if(err) {
                console.error(err)

                await interaction.reply("Internal Server Error")
                return
            }
            
            const permissionLevel = row ? row.permLevel : 0
            if(!row)
                database.run("INSERT INTO users (userId, permLevel) VALUES (?, ?)", [interaction.user.id, 0])

            if(permissionLevel == 0) {
                await interaction.reply({ content: "Only members of the SMTC Minecraft Server Community can use this bot!", flags: MessageFlags.Ephemeral })
                return
            }

            const serverInfo = await serverManager.getServerInfo(interaction)
            if(!serverInfo) {
                await interaction.reply({ content: "Invalid channel! Please use a channel thats assigned to a MC server!", flags: MessageFlags.Ephemeral })
                return
            }

            for(const command of this.commands) {
                if(command.data.name != interaction.commandName)
                    continue

                if(command.permissionLevel > permissionLevel) {
                    await interaction.reply({ content: "You do not have permission to run this command!", flags: MessageFlags.Ephemeral})
                    return
                }

                command.execute(interaction, serverInfo, permissionLevel)
                return
            }

            await interaction.reply({ content: "Unknown command", flags: MessageFlags.Ephemeral })
        })
    },
}