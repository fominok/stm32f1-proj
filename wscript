top = '.'
out = 'build'

build_obj = '${CC} ${CFLAGS}'

def configure(ctx):
    ctx.find_program('arm-none-eabi-gcc', var='CC')
    ctx.find_program('arm-none-eabi-objcopy', var='CP')
    ctx.find_program('arm-none-eabi-as')

    ctx.env['CFLAGS'] = '-c'
    ctx.find_program('gcc')

def build(ctx):
    src = **/*.ctx.path.find_node('src')
    ctx(rule='gcc ${CFLAGS} ${SRC}', source=ctx.path.ant_glob('**/src/*.c'))

