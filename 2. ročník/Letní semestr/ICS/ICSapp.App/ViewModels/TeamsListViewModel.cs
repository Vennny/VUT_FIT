using ICSapp.App.Commands;
using ICSapp.App.Views.Windows;
using ICSapp.BL.Messages;
using ICSapp.BL.Models;
using ICSapp.BL.Repositories;
using ICSapp.BL.Services;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Input;

namespace ICSapp.App.ViewModels
{
    public class TeamsListViewModel : BaseViewModel
    {

        private readonly IUserRepository userRepository;
        private readonly ITeamRepository teamRepository;
        private readonly IPostRepository postRepository;
        private readonly ICommentRepository commentRepository;


        public TeamModel Team { get; set; }
        public UserModel User { get; set; }
        public PostModel Post { get; set; }
        public List<TeamModel> TeamsList { get; set; }
        public List<CommentModel> CommentsList { get; set; }
        public List<PostModel> PostsList { get; set; }

        public ICommand AddCommentCommand { get; set; }
        public ICommand LoadPostCommand { get; set; }
        public ICommand CreatePostCommand { get; set; }
        public ICommand ActivateTeamCommand { get; set; }
        public ICommand CreateTeamCommand { get; set; }
        public ICommand RefreshTeamsWindowCommand { get; set; }

        private readonly IMediator mediator;

        public TeamsListViewModel(IUserRepository userRepository, IPostRepository postRepository, ITeamRepository teamRepository, ICommentRepository commentRepository, IMediator mediator)
        {
            this.userRepository = userRepository;
            this.postRepository = postRepository;
            this.teamRepository = teamRepository;
            this.commentRepository = commentRepository;

            this.mediator = mediator;
            User = new UserModel();
            Team = new TeamModel();
            Post = new PostModel();

            AddCommentCommand = new RelayCommand(OpenCreateComment);
            LoadPostCommand = new RelayCommand(LoadPostById);
            CreatePostCommand = new RelayCommand(CreateNewPost);
            ActivateTeamCommand = new RelayCommand(ActivateTeam);
            CreateTeamCommand = new RelayCommand(OpenCreateTeamWindow);
            RefreshTeamsWindowCommand = new RelayCommand(RefreshTeams);

            mediator.Register<ActiveUserMessage>(GetUserAndTeams);
        }

        private void OpenCreateComment(object obj)
        {

        }

        private void CreateNewPost(object obj)
        {
            Post.Author = User;
            Post.Time = DateTime.Now;
            if (string.IsNullOrEmpty(Post.Title) ||
                string.IsNullOrEmpty(Post.MessageContent)){

            }
            else
            {
                CreatePost();
                AddPost();
                object ob = (Team.Id) as object;
                ActivateTeam(ob);
            }
        }

        private void ActivateTeam(object obj)
        {
            Guid t = (Guid)obj;

            LoadTeamById(t);
            LoadPostsByTeam(Team.Id);
        }

        private void RefreshTeams(object obj)
        {
            mediator.Send(new ActiveUserMessage { Id = User.Id });
        }

        private void OpenCreateTeamWindow(object obj)
        {
            CreateTeamWindow NewTeamWindow = new CreateTeamWindow();
            NewTeamWindow.Show();

            mediator.Send(new ActiveUserMessage { Id = User.Id });
        }
        private void GetUserAndTeams(ActiveUserMessage obj)
        {
            LoadUserById(obj.Id);
            LoadTeamsByUserId(obj.Id);
        }

        public void LoadTeamsByUserId(Guid id)
        {
            TeamsList = userRepository.GetTeamsByUser(id);
        }

        public void LoadUserById(Guid id)
        {
            User = userRepository.GetById(id);
        }
        public void LoadTeamById(Guid id)
        {
            Team = teamRepository.GetById(id);
        }
        public void CreatePost()
        {
            Post = teamRepository.CreatePost(Post, User.Id);
        }
        public void AddPost()
        {
            Team = teamRepository.AddPost(Team.Id, Post.Id);
        }
        public void LoadPostsByTeam(Guid id)
        {
            PostsList = postRepository.GetPostsByTeam(id);
        }
        public void LoadPostById(object obj)
        {
            Guid id = (Guid)obj;
            Post = postRepository.GetById(id);
        }
        public void CreateComment()
        {

            //Post = postRepository.AddComment(Post.Id, Comment.Id);
        }
    }
}
