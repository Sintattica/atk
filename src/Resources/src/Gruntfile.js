module.exports = function (grunt) {

    grunt.initConfig({
        copy: {
            dist: {
                files: [
                    {
                        expand: true,
                        cwd: 'bower_components/bootstrap-sass/assets/fonts/',
                        src: ['**'],
                        dest: '../public/fonts/'
                    },
                    {
                        expand: true,
                        cwd: 'bower_components/font-awesome-sass/assets/fonts/',
                        src: ['**'],
                        dest: '../public/fonts/'
                    },
                    {
                        expand: true,
                        cwd: '.tmp/',
                        src: ['style.css.map'],
                        dest: '../public/styles/'
                    },
                    {
                        expand: true,
                        cwd: 'bower_components/smartmenus/src/addons/bootstrap/',
                        src: ['**/*.css'],
                        dest: 'bower_components/smartmenus/scss/',
                        rename: function (dest, src) {
                            return dest + '_' + src.replace(/\.css$/, ".scss");
                        }
                    }
                ]
            }
        },
        sass: {
            dist: {
                options: {
                    sourceMap: true
                },
                files: {
                    '../public/styles/style.css': 'sass/style.scss'
                }
            }
        },
        uglify: {
            dist: {
                options: {
                    beautify: false,
                    preserveComments: 'some'
                },
                files: {
                    '../public/javascript/atk.min.js': [
                        'bower_components/jquery/dist/jquery.js',
                        'bower_components/bootstrap-sass/assets/javascripts/bootstrap.js',
                        'bower_components/smartmenus/src/jquery.smartmenus.js',
                        'bower_components/smartmenus/src/addons/bootstrap/jquery.smartmenus.bootstrap.js',
                        'bower_components/moment/min/moment-with-locales.js',
                        'js/compatibility.js'
                    ]
                }
            }
        },
        watch: {
            css: {
                files: ['sass/*.scss'],
                tasks: ['sass'],
                options: {
                    livereload: true
                }
            }
        }
    });

    grunt.loadNpmTasks('grunt-contrib-copy');
    grunt.loadNpmTasks('grunt-sass');
    grunt.loadNpmTasks('grunt-contrib-uglify');
    grunt.loadNpmTasks('grunt-contrib-watch');

    grunt.registerTask('default', ['copy', 'sass', 'uglify']);
};