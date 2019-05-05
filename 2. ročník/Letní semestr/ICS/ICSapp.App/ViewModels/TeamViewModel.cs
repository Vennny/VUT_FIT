using ICSapp.App.Commands;
using ICSapp.App.Views.Windows;
using ICSapp.BL.Factories;
using ICSapp.BL.Messages;
using ICSapp.BL.Models;
using ICSapp.BL.Repositories;
using ICSapp.BL.Services;
using System;
using System.Collections.Generic;
using System.Windows.Input;

namespace ICSapp.App.ViewModels
{
    public class TeamViewModel
    {
        private readonly ITeamRepository teamRepository;
        private readonly IUserRepository userRepository;

        public List<UsersListModel> UsersList;
        public TeamModel Team;
        public UserModel User;
        public PostModel Post;

        public ICommand AddUserToTeamCommand { get; set; }
        

        public ICommand OpenNewTeamCommand { get; set; }
        //private readonly IMediator mediator;

        public string AddUserText { get; set; }
        public string AddUserEmail { get; set; }

        public TeamViewModel(IUserRepository userRepository, ITeamRepository teamRepository, IMediator mediator)
        {
            this.teamRepository = teamRepository;
            this.userRepository = userRepository;

            Team = new TeamModel();
            User = new UserModel();
            Post = new PostModel();

            AddUserToTeamCommand = new RelayCommand(TryAddUserToTeam);
        }

        private void TryAddUserToTeam(object obj)
        {
            if (string.IsNullOrEmpty(AddUserEmail))
            {
                AddUserText = "Field must not be empty";
            }
            LoadUserByEmail(AddUserEmail);
            if (User != null)
            {
                throw new NotImplementedException();
            }
        }
        public void LoadUsersByTeamId(Guid id)
        {
            UsersList = teamRepository.GetUsersByTeam(id);
        }
        public void LoadTeamById(Guid id)
        {
            Team = teamRepository.GetById(id);
        }
        public void AddPersonToTeam()
        {
            Team = teamRepository.AddTeamMate(Team.Id, User.Id);
        }
        public void RemoveTeam()
        {
            teamRepository.RemoveTeam(Team.Id);
        }
        public void RemovePersonFromTeam()
        {
            Team = teamRepository.RemoveTeamMate(Team.Id, User.Id);
        }
        public void CreatePost()
        {
            Post = teamRepository.CreatePost(Post, User.Id);
        }
        public void AddPost()
        {
            Team = teamRepository.AddPost(Team.Id, Post.Id);
        }
        public void LoadUserByEmail(String EmailAddress)
        {
            User = userRepository.GetByEmailAdress(EmailAddress);
        }
    }
}
