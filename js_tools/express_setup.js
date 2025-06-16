
import { mkdir, open } from 'fs/promises';

const createFolder = async (foldername) => {
  try {
    await mkdir(foldername, { recursive: false });
    console.log(`Folder ${foldername} created successfully!`);
  } catch (err) {
    console.error(`Error creating folder ${foldername}:`, err);
  }
}

const createFile = async (filename, contents) => {
  try {
    // 'wx' flag means "write, fail if exists"
    const fileHandle = await open(filename, 'wx');
    await fileHandle.writeFile(contents, 'utf-8');
    await fileHandle.close();
    console.log(`File ${filename} written successfully`);
  } catch (err) {
    if (err.code === 'EEXIST') {
      console.error(`File ${filename} already exists, not overwriting.`);
    } else {
      throw err;
    }
  }
}


(async () => {

  const ROOT_FOLDER_NAME = 'Server'
  await createFolder(ROOT_FOLDER_NAME)
  process.chdir(ROOT_FOLDER_NAME)
  await createFile('.env', '')
  await createFile('.gitignore', `
# Node modules folder (dependencies)
node_modules/

# Logs
logs/
*.log
npm-debug.log*
yarn-debug.log*
yarn-error.log*

# Environment variables
.env
.env.local
.env.*.local

# Debugger files
.vscode/
.idea/

# OS generated files
.DS_Store
Thumbs.db

# Build output (if you use a build step)
dist/
build/

# Optional npm cache folder
.npm/

# Optional coverage reports
coverage/

# Optional lock files if you want (usually you commit these)
# package-lock.json
# yarn.lock
  
  `)
  await createFile('package.json', `
{
  "scripts": {
    "dev": "ts-node-dev --respawn --transpile-only src/server.ts",
    "build": "tsc --project tsconfig.build.json",
    "start": "node dist/server.js"
  }    
}
  
  `)
  await createFile('tsconfig.json', `
{
  "compilerOptions": {
    "target": "ES2020",
    "module": "commonjs",
    "outDir": "dist",
    "rootDir": "src",
    "strict": true,
    "esModuleInterop": true,
    "forceConsistentCasingInFileNames": true
  }
}
  
  `)
  await createFile('tsconfig.build.json', `
{
  "extends": "./tsconfig.json",
  "compilerOptions": {
    "noEmit": false
  },
  "include": ["src"]
}
  `)
  await createFile('nodemon.json', `
{
  "watch": ["src"],
  "ext": "ts",
  "ignore": ["src/**/*.spec.ts"],
  "exec": "ts-node-dev src/server.ts"
}
  
  `)
  await createFile('README.md', `
  First run these two commands:
    - npm install express dotenv
    - npm install -D typescript ts-node-dev @types/node @types/express

  `)

  await createFolder('src')
  process.chdir('src')
  await createFolder('controllers')
  await createFolder('routes')
  await createFolder('middlewares')
  await createFolder('models')
  await createFolder('services')
  await createFolder('utils')
  await createFolder('config')
  await createFile('app.ts', `
import express from 'express'
import userRoutes from './routes/user.routes'

const app = express()

app.use(express.json())
app.use('/api/users', userRoutes)

export default app
  
  `)
  await createFile('server.ts', `
import app from './app'
import dotenv from 'dotenv'

dotenv.config()

const PORT = process.env.PORT || 3000

app.listen(PORT, () => {
  console.log(\`Server running on port \$\{PORT\}\`)
})
    `)

    process.chdir('./routes')
    await createFile('user.routes.ts', `
  import { Router } from 'express'
  import { getUsers } from '../controllers/user.controller'

  const router = Router()

  router.get('/', getUsers)

  export default router
  `)

  process.chdir('../controllers')
  await createFile('user.controller.ts', `
import { Request, Response } from 'express'

export const getUsers = (_req: Request, res: Response) => {
  res.json([{ id: 1, name: 'Alice' }, { id: 2, name: 'Bob' }])
}    
    `)
})()



